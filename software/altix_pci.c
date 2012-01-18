#include "lib/pci.h"
#include "lib/util.h"
#include "lib/pci_lib.h"
#include <linux/jiffies.h>

///PCI IDs defined in config.h
static struct pci_device_id ids[] =
{
	{ PCI_DEVICE(ALTIX_VEN_ID, ALTIX_DEV_ID), },
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, ids);

///Root node for the altix pci cards. Contains a linked list of altix_pci_node.
static altix_pci_head * altix_pci_root;

/**
 * Read the card ID from the PCI Card.
 * To be implemented once we move to new hardware. In the mean time
 * its sequentially assigned
 */
static uint read_card_id(altix_pci_node* node);


///Returns a card node by the card ID.
static inline altix_pci_node * get_node_by_id(int id);

///PCI function data structure. See probe and remove.
struct pci_driver pci_driver =
{
	.name = "pci_altix",
	.id_table = ids,
	.probe = probe,
	.remove = remove,
};

static inline altix_pci_node * get_node_by_id(int id)
{
	altix_pci_node * node = altix_pci_root->root;
	if(node == NULL)
		return NULL;
	while(node->next != NULL)
	{
		if(node -> id == id)
			break;
		else
			node = node->next;
	}
	if(node -> id != id)	//fall through case
		return NULL;
	else
		return node;
}

/**
 * Interupt handler, Wakes up the read for the card.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static irqreturn_t altix_int_svc( int irq, void *dev_id, struct pt_regs * regs)
#else
static irqreturn_t altix_int_svc( int irq, void *dev_id )
#endif
{
	int status;
	altix_pci_node *node = (altix_pci_node *)dev_id;
	altix_reg_check_irq(node, &status);
	if(status == 0)
		return IRQ_NONE;
	altix_reg_unset_dma(node);
	if (waitqueue_active(node->rd_waitq))
	{
		node->irq_flag = 1;
		wake_up_interruptible(node->rd_waitq);
	}
	return (IRQ_HANDLED);
}

uint read_card_id(altix_pci_node* node)
{
	uint id = 0;
	memcpy_fromio(&id, node->pcimemstartremap, sizeof(int));
	printk(KERN_INFO "Altix Driver registered device with ID: %x\n", id);
	return id;
}

int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	//Error code used through out the function.
	int err;
	//New node.
	altix_pci_node* temp_node;
	altix_pci_node * new_node = kmalloc(sizeof(altix_pci_node), GFP_KERNEL);
	//Set up the atomic lock
	clear_bit(0,&(new_node-> node_lock));
	//Bring down the root semaphore, only one probe has access to the root at a time
	while(down_interruptible(altix_pci_root->probe_lock)) {}
	//Enable the device
	err=pci_enable_device( dev );
	if (err)
	{
		printk(KERN_ERR "altix CPCI Driver: failed, PCI Enable Device returned err=%d\n", err);
		goto probe_fail;
	}
	//Rename to a name which makes sence.
	printk("altix CPCI Driver: Old device name: \"%s\"\n", dev->dev.kobj.name);
	snprintf((char*)(dev->dev.kobj.name), KOBJ_NAME_LEN, "ALTIX PCI #%d", altix_pci_root->length);
	//Initialize the config memory bar
	new_node->config_pcimemlen = pci_resource_len(dev, ALTIX_CONFIG_BAR);
	new_node->config_pcimemstart = pci_resource_start(dev, ALTIX_CONFIG_BAR);
	//Sanity checks
	if(new_node->config_pcimemlen == 0)
	{
		printk(KERN_ERR "altix CPCI Driver: PCI config memory was not initialized properly!\n");
		goto probe_fail;
	}
	if(check_mem_region(new_node->config_pcimemstart, new_node->config_pcimemlen) != 0)
	{
		printk(KERN_ERR "altix CPCI Driver: config memory region is already in use..erhmm, something is wrong here!\n");
		goto probe_fail;
	}
	request_mem_region(new_node->config_pcimemstart, new_node->config_pcimemlen, "altix");
	new_node->config_pcimemstartremap = ioremap(new_node->config_pcimemstart, new_node->config_pcimemlen);
	printk("altix CPCI Driver: Allocated config region %d at: 0x%lx with length 0x%lx\n", ALTIX_CONFIG_BAR, new_node->config_pcimemstart, new_node->config_pcimemlen);
	//Initialize the data memory bar
	new_node->pcimemlen = pci_resource_len(dev, ALTIX_PCI_BAR);
	new_node->pcimemstart = pci_resource_start(dev, ALTIX_PCI_BAR);
	//Sanity checks
	if(new_node->pcimemlen == 0)
	{
		printk(KERN_ERR "altix CPCI Driver: PCI memory was not initialized properly!\n");
		goto probe_fail;
	}
	if(check_mem_region(new_node->pcimemstart, new_node->pcimemlen) != 0)
	{
		printk(KERN_ERR "altix CPCI Driver: memory region is already in use..erhmm, something is wrong here!\n");
		goto probe_fail;
	}
	request_mem_region(new_node->pcimemstart, new_node->pcimemlen, "chardev");
	new_node->pcimemstartremap = ioremap(new_node->pcimemstart, new_node->pcimemlen);
	printk("altix CPCI Driver: Allocated data region %d at: 0x%lx with length 0x%lx\n", ALTIX_PCI_BAR, new_node->pcimemstart, new_node->pcimemlen);

	new_node->rd_waitq = kmalloc(sizeof(wait_queue_head_t), GFP_KERNEL);
	init_waitqueue_head(new_node->rd_waitq);

	//Set bus master;
	pci_set_master(dev);
	//pci_write_config_dword(dev, PCI_CACHE_LINE_SIZE, 0x4020);
	//Finish setting up the kernel structures.
	new_node->io_lock = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	sema_init(new_node->io_lock, 1);
	new_node->next = NULL;
	new_node->id = read_card_id(new_node);
	new_node->pid = 0;
	new_node->num_reads = 0;
	new_node->num_writes = 0;
	new_node->bytes_read = 0;
	new_node->bytes_written = 0;
	new_node->irq_enabled = 0;
	new_node->dma_pool = NULL;
	new_node->dev = dev;
	new_node->irq_flag = 0;
	new_node->channel = 0;
	altix_reg_init(new_node);
	altix_reg_datalen( new_node );
	//Put the device into the global linked list structure.
	if(altix_pci_root->root == NULL)	//Empty Case
	{
		altix_pci_root->root = new_node;
		new_node->prev = NULL;
	}
	else 								//Non Empty Case
	{
		temp_node = altix_pci_root->root;
		while(temp_node->next != NULL)	//Itterate through the nodes.
		{
			temp_node = temp_node->next;
		}
		temp_node->next = new_node;
		new_node->prev = temp_node;
	}
	dev->sysdata = new_node;
	altix_pci_root->length++;
	up(altix_pci_root->probe_lock);
	return (0);
probe_fail:			//Failed Before IRQ request
	kfree(new_node);
	pci_disable_device( dev );
	up(altix_pci_root->probe_lock);
	return (-EIO);
}

void remove(struct pci_dev *dev)
{
	altix_pci_node * remove_node;
	//If DMA is pending release the sleeping process
	//Lock the datastructure
	while(down_interruptible(altix_pci_root->probe_lock)) {}
	//Get to the datastructure
	remove_node = (altix_pci_node*)dev->sysdata;
	printk(KERN_INFO "altix CPCI Driver: remove called on ID: %d\n", remove_node->id);
	//Disable the Interup line
	if(remove_node->irq_enabled && dev->irq!=0)
	{
		altix_reg_disable_irq(remove_node);
		free_irq( dev->irq, remove_node );
	}
	//Check for sleeping processes
	if (waitqueue_active(remove_node->rd_waitq))
	{
		remove_node->irq_flag = 1;
		wake_up_interruptible(remove_node->rd_waitq);
	}
	kfree(remove_node->rd_waitq);
	if(remove_node->dma_pool != NULL)
	{
		altix_dma_pool_free(remove_node->dma_pool, dev);
	}
	altix_reg_remove(remove_node);
	remove_node->dma_pool = NULL;
	//Unmap the memory
	if(remove_node->pcimemlen != 0)
		release_mem_region(remove_node->pcimemstart, remove_node->pcimemlen);
	if(remove_node->config_pcimemlen != 0)
		release_mem_region(remove_node->config_pcimemstart, remove_node->config_pcimemlen);
	//Remove the datastructure from the internal linked list structure
	altix_pci_root->length--;
	//PREV
	if(remove_node->prev == NULL)
	{
		//ROOT node
		altix_pci_root->root = remove_node->next;
	}
	else
	{
		//Not root node
		remove_node->prev->next = remove_node->next;
	}
	//NEXT
	if(remove_node->next != NULL)
	{
		//Not Last node
		remove_node->next->prev = remove_node->prev;
	}
	//free up the resourcess;
	kfree(remove_node->io_lock);
	kfree(remove_node);
	//NULL the private data;
	dev->sysdata = NULL;
	up(altix_pci_root->probe_lock);
	return;
}

int altix_pci_get_cards_number()
{
	//Just grab it from th data structure
	return altix_pci_root->length;
}

altix_pci_card_info* altix_pci_get_cards()
{
	int i;
	altix_pci_card_info* data;
	altix_pci_node *node;
	//Avoid kmalloc(0);
	if(altix_pci_root->length == 0)
		return NULL;
	//Allocate data array
	data = kmalloc(sizeof(altix_pci_card_info)*altix_pci_root->length, GFP_KERNEL);
	//Zero out the data
	memset(data, 0x00, sizeof(altix_pci_card_info)*altix_pci_root->length);
	node = altix_pci_root->root;
	for(i = 0; i< altix_pci_root->length; i++)
	{
		//Itterate over the datastructure and gather appropriate data.
		data[i].pid = node->pid;
		data[i].memlen	= node->pcidatalen;
		data[i].id = node->id;
		data[i].channel = node->channel;
		//Bulletproof the driver in case altix_pci_root->length is not correct
		if(node->next != NULL)
			node = node->next;
		else
			break;
	}
	return data;
}

altix_pci_card_stat * altix_pci_get_card_stats()
{
	int i;
	altix_pci_card_stat* data;
	altix_pci_node *node;
	//Avoid kmalloc(0);
	if(altix_pci_root->length == 0)
		return NULL;
	//Allocate data array
	data = kmalloc(sizeof(altix_pci_card_stat)*altix_pci_root->length, GFP_KERNEL);
	memset(data, 0x00, sizeof(altix_pci_card_stat)*altix_pci_root->length);
	node = altix_pci_root->root;
	for(i = 0; i< altix_pci_root->length; i++)
	{
		//Iterate over the datastructure and gather appropriate data.
		data[i].num_reads = node->num_reads;
		data[i].num_writes	= node->num_writes;
		data[i].bytes_read = node->bytes_read;
		data[i].bytes_written = node->bytes_written;
		data[i].id = node->id;
		//Bulletproof the driver in case altix_pci_root->length is not correct
		if(node->next != NULL)
			node = node->next;
		else
			break;
	}
	return data;
}

unsigned long altix_pci_get_memlen_by_id(int id)
{
	altix_pci_node *node;
	//itterate from the root node till the id is found
	node = get_node_by_id(id);
	if(node == NULL)
		return 0;
	return node->pcidatalen;
}


int altix_pci_lock_by_id(int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if (node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(test_and_set_bit(0, &(node->node_lock)) != 0) //try to lock
		return ALTIX_PCI_CARD_BUSY;
	node->pid = current->pid;			//If locked, mark it by the process
	return ALTIX_PCI_CARD_OK;
}

int altix_pci_release_by_id(int id)
{
	altix_pci_node* node = get_node_by_id(id);
	if(node == NULL ) return ALTIX_PCI_CARD_UNAVAILABLE;
	if( node -> pid != current->pid ) return ALTIX_PCI_CARD_BUSY;
	node->pid = 0;
	clear_bit(0,&(node-> node_lock));
	return ALTIX_PCI_CARD_OK;
}
int altix_pci_unconditional_release_by_id(int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if (node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	node->pid = 0;
	clear_bit(0,&(node-> node_lock));
	return ALTIX_PCI_CARD_OK;
}
//This function reads up to the end of pci memory and
//then returns the number of bytes read
int altix_pci_readregion_by_id(void* data, int offset, int len, int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node -> pid != current->pid)	//card not locked by the process
	{
		printk(KERN_ERR "altix CPCI Driver: Tried to read %d, but i am locked by %d\n", (int)current->pid, (int)node->pid);
		return ALTIX_PCI_CARD_BUSY;
	}
	if(offset >= node->pcidatalen)
		return 0;
	if(offset + len > node->pcidatalen)
		len = node->pcidatalen - offset;
	offset = offset + node->pcidataoffset;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	memcpy_fromio(data, node->pcimemstartremap+offset, len);
	node->num_reads++;
	node->bytes_read +=len;
	up(node->io_lock);
	return len;
}

//This function writes up to the end of pci memory and
//then returns the number of bytes writen
int altix_pci_writeregion_by_id(void* data, int offset, int len, int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(offset >= node->pcidatalen)
		return 0;
	if(offset + len >= node->pcidatalen)
		len = node->pcidatalen - offset;
	offset = offset + node->pcidataoffset;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	node->num_writes++;
	node->bytes_written +=len;
	memcpy_toio(node->pcimemstartremap+offset, data, len);
	up(node->io_lock);
	return len;
}

//DMA read will sleep until interrupt from the pci card.
int altix_pci_dma_read_by_id(void __user *data, int len, int id)
{
	int error;
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	if(node->dma_pool == NULL)
	{
		node->dma_pool = altix_dma_pool_init(node->dev);
	}
	if(node->irq_enabled == 0)
	{
		if((node->dev->irq!=0) && (request_irq(node->dev->irq, &altix_int_svc, SA_INTERRUPT|SA_SHIRQ, "altix", node)))
		{
			printk(KERN_ERR "altix CPCI Driver: IRQ %d not free.\n", node->dev->irq );
			return ALTIX_PCI_CARD_UNAVAILABLE;
		}
		else
		{
			altix_reg_enable_irq(node);
			node->irq_enabled = 1;
		}
	}
	node->dma_handle = altix_dma_pool_alloc(node->dev, len, node->dma_pool);
	altix_reg_set_dma(node, len, node->channel, read_dir);
	//sleep till interrupted or timeout
	node->irq_flag = 0;
	error = wait_event_interruptible_timeout(*node->rd_waitq, node->irq_flag == 1, msecs_to_jiffies(1000));
	if(error < 0)
	{
		printk(KERN_ERR "altix_driver: wait_event_interruptible_timeout interrupted with status %d during dma read\n", error);
		node->irq_flag = 0;
		altix_reg_unset_dma(node);
		altix_reg_clear_irq(node, &len);
		up(node->io_lock);
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	if(node->irq_flag == 0)
	{
		printk(KERN_ERR "altix driver: Interrupt not recieved after 1 sec, giving up on reading this packet\n");
		altix_reg_unset_dma(node);
		altix_reg_clear_irq(node, &len);
		up(node->io_lock);
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	node->irq_flag = 0;
	altix_reg_clear_irq(node, &len);
	len = altix_dma_pool_read(node->dma_pool, len, data);
	node->num_reads++;
	node->bytes_read +=len;
	up(node->io_lock);
	return len;
}

int altix_pci_dma_write_by_id(void __user* data, int len, int id)
{
	int error;
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	if(node->dma_pool == NULL)
	{
		node->dma_pool = altix_dma_pool_init(node->dev);
	}
	if(node->irq_enabled == 0)
	{
		if((node->dev->irq!=0) && (request_irq(node->dev->irq, &altix_int_svc, SA_INTERRUPT|SA_SHIRQ, "altix", node)))
		{
			printk(KERN_ERR "altix CPCI Driver: IRQ %d not free.\n", node->dev->irq );
			return ALTIX_PCI_CARD_UNAVAILABLE;
		}
		else
		{
			altix_reg_enable_irq(node);
			node->irq_enabled = 1;
		}
	}
	node->dma_handle = altix_dma_pool_alloc(node->dev, len, node->dma_pool);
	len = altix_dma_pool_write(node->dma_pool, len, data);
	altix_reg_set_dma(node, len, node->channel, write_dir);
	//sleep till interrupted or timeout
	node->irq_flag = 0;
	error = wait_event_interruptible_timeout(*node->rd_waitq, node->irq_flag == 1, msecs_to_jiffies(10000));
	if(error < 0)
	{
		printk(KERN_ERR "altix_driver: wait_event_interruptible_timeout interrupted with status %d during write\n", error);
		node->irq_flag = 0;
		altix_reg_unset_dma(node);
		altix_reg_clear_irq(node, &len);
		up(node->io_lock);
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	if(node->irq_flag == 0)
	{
		printk(KERN_ERR "altix driver: Interrupt not recieved after 10 sec, giving up on writing this packet\n");
		altix_reg_unset_dma(node);
		altix_reg_clear_irq(node, &len);
		up(node->io_lock);
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	node->irq_flag = 0;
	altix_reg_clear_irq(node, &len);
	node->num_writes++;
	node->bytes_written +=len;
	up(node->io_lock);
	return len;
}

int altix_pci_disable_irq_by_id(int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->irq_enabled)
	{
		if(waitqueue_active(node->rd_waitq))
			return ALTIX_PCI_CARD_BUSY;
		if (node->dev->irq!=0)
			free_irq( node->dev->irq, node );
		altix_reg_disable_irq(node);
	}
	node->irq_enabled = 0;
	return ALTIX_PCI_CARD_OK;
}

unsigned int altix_pci_poll(struct file* filp, unsigned int card_id, poll_table *wait)
{
	altix_pci_node* node = get_node_by_id(card_id);
	unsigned int mask = 0;

	if (node == NULL)
		return mask;

	//pci node only has one queue
	poll_wait( filp, node->rd_waitq, wait);
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	altix_reg_poll(node, &mask);
	up(node->io_lock);
	return mask;
}

int altix_pci_set_channel(int id, int chan)
{
	altix_pci_node *node = get_node_by_id(id);

	if(chan > 3 || chan < 0)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	node->channel = chan;
	up(node->io_lock);
	return ALTIX_PCI_CARD_OK;
}

int altix_pci_enable_channel(int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	altix_reg_enable_chan(node);
	up(node->io_lock);
	return ALTIX_PCI_CARD_OK;
}

int altix_pci_disable_channel(int id)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	altix_reg_disable_chan(node);
	up(node->io_lock);
	return ALTIX_PCI_CARD_OK;
}

int altix_pci_channel_status(int id, int *status)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	altix_reg_chan_status(node, status);
	up(node->io_lock);
	return ALTIX_PCI_CARD_OK;
}

int altix_pci_fancy(int id, int cmd, int * ret)
{
	altix_pci_node *node = get_node_by_id(id);
	if(node == NULL)
		return ALTIX_PCI_CARD_UNAVAILABLE;
	if(node->pid != current->pid)
		return ALTIX_PCI_CARD_BUSY;
	if(down_interruptible(node->io_lock) < 0)
	{
		return ALTIX_PCI_CARD_IO_INTERRUPTED;
	}
	altix_reg_fancy(node, cmd, ret);
	up(node->io_lock);
	return ALTIX_PCI_CARD_OK;
}

void altix_exit_pci(void)
{
	printk(KERN_INFO "Exiting the PCI driver.\n");
	pci_unregister_driver(&pci_driver);
	kfree(altix_pci_root->probe_lock);
	kfree(altix_pci_root);
	printk(KERN_INFO "Freeing the PCI library \n");
	altix_pci_lib_free();
}

int altix_init_pci(void)
{
	int err;
	int i;
	altix_pci_lib_init();
	printk(KERN_INFO "	Initializing the pci library...");
	for(i = 0; i< pci_lib_size; i++)
	{
		printk(KERN_INFO  "		Device %d : %s\n", i , pci_lib[i]->name);
	}
	printk(KERN_INFO "	Done\n");
	altix_pci_root = kmalloc(sizeof(altix_pci_head),GFP_KERNEL);
	altix_pci_root->length = 0;
	altix_pci_root->root = NULL;
	altix_pci_root->probe_lock = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	sema_init(altix_pci_root->probe_lock, 1);
	err = pci_register_driver(&pci_driver);
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
	if (err!=0)
	{
		printk(KERN_ERR "altix CPCI Driver: error %d in pci_register_driver\n", err);
		return err;
	}
	#endif
	return 0;
}
