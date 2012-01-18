/**
 * \file altix_chardev.c
 * Implementation file for the character device subsystem. These functions provide a well defined access from userland to kernel.
 */

#include "lib/chardev.h"
#include "lib/pci.h"

/**
 * Seeks to another position. Does nothing if DMA is enabled. If DMA is disabled , it will seek anywhere within the PCI bar, except for the DMA registers.
 */
loff_t altix_chardev_lseek(struct file *file, loff_t offset, int orig);

/**
 * Open a file descriptor for use. Sets up the file descriptor and hides altix_chardev_node in the private date of the file descriptor.
 */
int altix_chardev_open(struct inode *inode,struct file *filep);

/**
 * Close a file descriptor and release the card. If DMA is enabled driver will attempt to disable the IRQ on the locked card, before releasing it.
 */
int altix_chardev_release(struct inode *inode,struct file *filep);

/**
 * Read data from file descriptor. May return an error if card is currently locked by another process, or the read gets interrupted
 */
ssize_t altix_chardev_read(struct file *filep,char __user * buff, size_t count, loff_t *offp );

/**
 * Write data to file descriptor. May return an error if card is already locked by another process. Depending on the descriptor state IO will come from the pci bar ot DMA pool.
 */
ssize_t altix_chardev_write(struct file *filep,const char __user* buff, size_t count, loff_t *offp );

/**
 * Miscellaneous device calls for PCI subsystem: switch channels, get number of cards, get information about connected cards, lock/release cards, get stats from cards.
 */
#if HAVE_UNLOCKED_IOCTL == 1
static long altix_chardev_ioctl(struct file* filep , unsigned int cmd , unsigned long  arg);
#else
long altix_chardev_ioctl(struct inode *inode, struct file* filep , unsigned int cmd , unsigned long  arg);
#endif


/**
 * Polls the locked card/channel to check the availability of readable data. Also check the output FIFO for available space.
 */
unsigned int altix_chardev_poll(struct file *filp, poll_table *wait);

/// File operations for the chararcter device
struct file_operations altix_chardev_fops;

///Major number.
int altix_chardev_major=-1;


///The cdev structure.
struct cdev cdev;
static dev_t chardev_first;

int altix_init_chardev(void)
{
	altix_chardev_fops.open =  altix_chardev_open;
	altix_chardev_fops.read = altix_chardev_read;
	altix_chardev_fops.write = altix_chardev_write;
	altix_chardev_fops.release = altix_chardev_release;
	altix_chardev_fops.llseek = altix_chardev_lseek;
	#if HAVE_UNLOCKED_IOCTL == 1
	altix_chardev_fops.unlocked_ioctl = altix_chardev_ioctl;
	#else
	altix_chardev_fops.ioctl = altix_chardev_ioctl;
	#endif
	altix_chardev_fops.poll = altix_chardev_poll;

	if(alloc_chrdev_region (&chardev_first,0,1,"altix_chardev")<0)
	{
		printk(KERN_ERR "altix chardev: couldnt get major/minor number.\n");
		return -EIO;
	}
	altix_chardev_major=MAJOR(chardev_first);
	cdev_init ( &cdev, &altix_chardev_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &altix_chardev_fops;
	if(cdev_add(&cdev, MKDEV(altix_chardev_major, 0), 1) < 0)
	{
		printk(KERN_ERR "altix chardev: Failed to initialize character device\n");
		return -EIO;
	}
	printk(KERN_INFO "	altix chardev: Initialized character device with major number %d\n", altix_chardev_major);
	return 0;
}

void altix_exit_chardev(void)
{
	cdev_del(&cdev);
	unregister_chrdev_region (chardev_first,1);
}

//File operations


int altix_chardev_open(struct inode *inode,struct file *filep)
{
	altix_chardev_node* node = kmalloc(sizeof(altix_chardev_node), GFP_KERNEL);
	node->opened_card = ALTIX_CHARDEV_NO_CARD;
	node->pid = current->pid;
	node->dma_active = 0;
	filep->private_data = node;
	return 0;
}

int altix_chardev_release(struct inode *inode,struct file *filep)
{
	altix_chardev_node* node = filep->private_data;
	if(node->opened_card != ALTIX_CHARDEV_NO_CARD)
	{
		if(altix_pci_disable_irq_by_id(node->opened_card) == ALTIX_PCI_CARD_BUSY)
			return -EBUSY;
		altix_pci_unconditional_release_by_id(node->opened_card);
		node->opened_card = ALTIX_CHARDEV_NO_CARD;
	}
	kfree(node);
	return 0;
}

ssize_t altix_chardev_read(struct file *filep,char __user*buff,size_t count,loff_t *f_pos )
{
	altix_chardev_node* node = filep->private_data;
	void * udata;
	if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
	{
		return 0;
	}
	if(!access_ok(VERIFY_READ, buff, count))
	{
		printk(KERN_ERR "altix chardev: chardev_read, bad user buffer\n");
		return -EIO;
	}
	if(node->dma_active)
	{
		count &= (~0x3);
		if(count <= 0)
			return 0;
		count = altix_pci_dma_read_by_id(buff, count, node->opened_card);
		if(count <0)
			return -EIO;
	}
	else
	{
		if(count > altix_pci_get_memlen_by_id(node->opened_card))
			count = altix_pci_get_memlen_by_id(node->opened_card);
		udata  = kmalloc(sizeof(char)*count, GFP_KERNEL);
		memset(udata, 0x00, count);
		count = altix_pci_readregion_by_id(udata, filep->f_pos, count, node->opened_card);
		if (count <= 0)
		{
			kfree(udata);
			switch (count)
			{
			case 0:
				return 0;
				break;
			case ALTIX_PCI_CARD_BUSY:
				return -EBUSY;
				break;
			case ALTIX_PCI_CARD_IO_INTERRUPTED:
				return -ERESTARTSYS;
				break;
			default :
				;
			}
			return -ENODEV;
		}
		if((copy_to_user(buff, udata, count) != 0))
		{
			kfree(udata);
			printk(KERN_ERR "altix chardev: unable copy to user possble free corruption or NULL refirence in userland. \n");
			return -EIO;
		}
		kfree(udata);
		filep->f_pos += count;
	}
	return count;
}

ssize_t altix_chardev_write(struct file *filep,const char __user *buff,size_t count,loff_t *f_pos )
{
	altix_chardev_node* node = filep->private_data;
	void * udata;
	if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
	{
		return 0;
	}
	if(!access_ok(VERIFY_WRITE, buff, count))
	{
		printk(KERN_ERR "altix chardev: chardev_write, bad user buffer\n");
		return -EIO;
	}
	if(node->dma_active)
	{
		count &= (~0x3);
		if(count <= 0)
			return 0;
		count = altix_pci_dma_write_by_id((void __user *)buff, count, node->opened_card);
		if(count <0)
			return -EIO;
	}
	else
	{
		if(count > altix_pci_get_memlen_by_id(node->opened_card))
		{
			count = altix_pci_get_memlen_by_id(node->opened_card);
		}
		udata = kmalloc(sizeof(char)*count, GFP_KERNEL);
		if(copy_from_user(udata, buff, count) != 0)
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
		}
		count = altix_pci_writeregion_by_id(udata, *f_pos, count, node->opened_card);
		kfree(udata);
		if(count == ALTIX_PCI_CARD_IO_INTERRUPTED)
			return -ERESTARTSYS;
		if(count  == ALTIX_PCI_CARD_BUSY)
			return -EBUSY;
		if(count < 0)
			return -ENODEV;
		filep->f_pos += count;
	}
	return count;
}

loff_t altix_chardev_lseek(struct file *filep, loff_t off, int whence)
{
	loff_t newpos;
	altix_chardev_node * node= filep->private_data;
	if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
	{
		return -ENODEV;
	}
	switch(whence)
	{
	case 0: /* SEEK_SET */
		newpos = off;
		break;

	case 1: /* SEEK_CUR */
		newpos = filep->f_pos + off;
		break;

	case 2: /* SEEK_END */
		newpos = altix_pci_get_memlen_by_id(node->opened_card);
		break;

	default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0)
		return -EINVAL;
	if (newpos > altix_pci_get_memlen_by_id(node->opened_card))
		return -EINVAL;
	filep-> f_pos = newpos;
	return newpos;
}

#if HAVE_UNLOCKED_IOCTL == 1
static long altix_chardev_ioctl(struct file* filep , unsigned int cmd , unsigned long  arg)
#else
long altix_chardev_ioctl(struct inode *inode, struct file* filep , unsigned int cmd , unsigned long  arg)
#endif
{
	void __user * buff = (void*)arg;
	altix_chardev_node* node = filep->private_data;

	switch (cmd)
	{
		//Get Number of cards
	case  ALTIX_IOCTL_NUM:
	{
		int data = altix_pci_get_cards_number();
		if(!access_ok(VERIFY_WRITE, buff, sizeof(int32_t)))
		{
			printk(KERN_ERR "altix chardev: ALTIX_IOCTL_NUM, bad user buffer\n");
			return -EIO;
		}
		if( copy_to_user(buff, &data, sizeof(int32_t)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		break;
	}
	//Get List of cards:
	case ALTIX_IOCTL_INFO:
	{
		altix_pci_card_info* data = altix_pci_get_cards();
		int num_cards =  altix_pci_get_cards_number();
		if(!access_ok(VERIFY_WRITE, buff, num_cards*sizeof(altix_pci_card_info)))
		{
			printk(KERN_ERR "altix chardev: ATIX_IOCTL_INFO, bad user buffer\n");
			return -EIO;
		}
		if( copy_to_user(buff, data, num_cards*sizeof(altix_pci_card_info)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		kfree(data);
		break;
	}
	//Lock card by ID:
	case ALTIX_IOCTL_LOCK:
	{
		int card_id = 0;
		int err;
		if(node->opened_card != ALTIX_CHARDEV_NO_CARD)
		{
			altix_pci_release_by_id(node->opened_card);
			altix_pci_disable_irq_by_id(node->opened_card);
			node->opened_card = ALTIX_CHARDEV_NO_CARD;
		}
		card_id = arg;
		err = altix_pci_lock_by_id(card_id);
		switch (err)
		{
		case ALTIX_PCI_CARD_OK:
			node->opened_card = card_id;
			break;

		case ALTIX_PCI_CARD_UNAVAILABLE:
			return - ENODEV;

		case ALTIX_PCI_CARD_BUSY:
			return -EBUSY;
		}
		break;
	}
	case ALTIX_IOCTL_RELEASE:
	{
		int error;
		if(node->opened_card != ALTIX_CHARDEV_NO_CARD)
		{
			altix_pci_disable_irq_by_id(node->opened_card);
			error = altix_pci_release_by_id(node->opened_card);
			node->opened_card = ALTIX_CHARDEV_NO_CARD;
			if(error == ALTIX_PCI_CARD_UNAVAILABLE)
				return -ENODEV;
		}
		return 0;
	}
	case ALTIX_IOCTL_STAT:
	{
		altix_pci_card_stat* data = altix_pci_get_card_stats();
		int num_cards =  altix_pci_get_cards_number();
		if(!access_ok(VERIFY_WRITE, buff, num_cards*sizeof(altix_pci_card_stat)))
		{
			printk(KERN_ERR "altix chardev: ATIX_IOCTL_STAT, bad user buffer\n");
			return -EIO;
		}
		if( copy_to_user(buff, data, num_cards*sizeof(altix_pci_card_stat)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		kfree(data);
		break;
	}
	case ALTIX_IOCTL_DMA:
	{
		if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
			return -ENODEV;
		if(node->dma_active)
			node->dma_active = 0;
		else
			node->dma_active = 1;
		break;
	}
	case ALTIX_IOCTL_CHAN:
	{
		int card_id = node->opened_card;
		int new_chan = arg;
		int error;
		if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
			return -ENODEV;
		error =  altix_pci_set_channel(card_id, new_chan);
		if(error == ALTIX_PCI_CARD_UNAVAILABLE)
			return -ENODEV;
		if(error == ALTIX_PCI_CARD_IO_INTERRUPTED)
			return -ERESTART;
		break;
	}
	case ALTIX_IOCTL_CHAN_ENABLE:
	{
		int card_id = node->opened_card;
		int error;
		int on_off = arg;
		if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
			return -ENODEV;
		if(on_off)
			error = altix_pci_enable_channel(card_id);
		else
			error = altix_pci_disable_channel(card_id);
		if(error == ALTIX_PCI_CARD_UNAVAILABLE)
			return -ENODEV;
		if(error == ALTIX_PCI_CARD_IO_INTERRUPTED)
			return -ERESTART;
		break;
	}
	case ALTIX_IOCTL_CHAN_STATUS:
	{
		int32_t status;
		int error;
		int card_id = node->opened_card;
		if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
			return -ENODEV;
		error = altix_pci_channel_status(card_id, &status);
		if(error == ALTIX_PCI_CARD_UNAVAILABLE)
			return -ENODEV;
		if(error == ALTIX_PCI_CARD_IO_INTERRUPTED)
			return -ERESTART;
		if(!access_ok(VERIFY_WRITE, buff, sizeof(int32_t)))
		{
			printk(KERN_ERR "altix chardev: ATIX_IOCTL_STAT, bad user buffer\n");
			return -EIO;
		}
		if( copy_to_user(buff, &status, sizeof(int32_t)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		break;
	}
	case ALTIX_IOCTL_FANCY:
	{
		int cmd, ret, error;
		if(node->opened_card == ALTIX_CHARDEV_NO_CARD)
			return -ENODEV;
		if(!access_ok(VERIFY_WRITE, buff, sizeof(int32_t)))
		{
			printk(KERN_ERR "altix chardev: ALTIX_IOCTL_FANCY, bad user buffer\n");
			return -EIO;
		}
		if( copy_from_user(&cmd, buff, sizeof(int32_t)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		error = altix_pci_fancy(node->opened_card, cmd, &ret);
		if(error == ALTIX_PCI_CARD_UNAVAILABLE)
			return -ENODEV;
		if(error == ALTIX_PCI_CARD_IO_INTERRUPTED)
			return -ERESTART;
		if( copy_to_user(buff, &ret, sizeof(int32_t)) != 0 )
		{
			printk(KERN_ERR "altix chardev: copy from user failed\n");
			return -EIO;
		}
		break;
	}
	case ALTIX_IOCTL_VERSION:
	{
		int version = ALTIX_DRIVER_VERSION;
		if(!access_ok(VERIFY_WRITE, buff, sizeof(uint32_t)))
		{
			printk(KERN_ERR "altix chardev: ATIX_IOCTL_STAT, bad user buffer\n");
			return -EIO;
		}
		if(copy_to_user(buff, &version, sizeof(uint32_t)) != 0)
		{
			printk(KERN_ERR "altix chardev: copy to user failed\n");
			return -EIO;
		}
		break;
	}
	default:
		return -EINVAL;
		break;
	}
	return 0;
}

unsigned int altix_chardev_poll(struct file *filp, poll_table *wait)
{
	altix_chardev_node* node = filp->private_data;
	unsigned int mask = 0;

	if (node->opened_card != ALTIX_CHARDEV_NO_CARD)
		mask = altix_pci_poll(filp, node->opened_card, wait);
	return mask;
}
