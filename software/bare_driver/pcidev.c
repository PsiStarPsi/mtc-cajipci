#include "lib/pcidev.h"

extern struct blab_buffered_data* blab_data;

int probe(struct pci_dev *dev, const struct pci_device_id *id);

static struct pci_device_id ids[] = {
	{ PCI_DEVICE(BLAB_VEN_ID, BLAB_DEV_ID), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

struct pci_driver pci_driver = {
	.name = "pci_blab",
	.id_table = ids,
	.probe = probe,
	.remove = remove,
};

unsigned long pcimemstart,pcimemlen;
void *pcimemstartremap,*pcimemstartnocache;


static irqreturn_t blab_int_svc( int irq, void *dev_id )  //kernel headers says..  (int, void *)
{
	if(blab_data->stamp == 0)
	{
		return (IRQ_HANDLED);
	}
	blab_data->stamp++;
	if(blab_data->r_pos == blab_data->w_pos)
	{
		//Out of space, dropping packet!!
		return (IRQ_HANDLED);
	}
	memcpy_fromio(blab_data->data[blab_data->w_pos], pcimemstartremap, BUFFSIZE);
	blab_data->data_stamp[blab_data->w_pos] = blab_data->stamp++;
	blab_data->w_pos=(blab_data->w_pos+1)%BUFFNUM;
	up(blab_data->rw_sem);
    return (IRQ_HANDLED);
}

int blab_readregion_pci(void* data, int offset, int len)
//this function reads up to the end of pci memory and then returns the number of bytes read
{

	offset=offset%pcimemlen;

	if(offset+len > pcimemlen)
		len = pcimemlen-offset;
	//ioread32_rep(pcimemstartremap+offset, data, len/4);
	memcpy_fromio(data, pcimemstartremap+offset, len);

	return len;
}

int blab_writeregion_pci(void* data, int offset, int len)
//this function writes up to the end of pci memory
{
	offset=offset%pcimemlen;

	if(offset+len > pcimemlen)
		len = pcimemlen-offset;
	//iowrite32_rep(pcimemstartremap+offset, data, len/4);
	memcpy_toio(pcimemstartremap+offset, data, len);

	return len;
}

static unsigned char blab_get_revision(struct pci_dev *dev)
{
	u8 revision;

	pci_read_config_byte(dev, PCI_REVISION_ID, &revision);
	return revision;
}

int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	/* Do probing type stuff here.
	 * Like calling request_region();
	 */
	int err;

	printk(KERN_ERR "BLAB3 CPCI Driver: probe..\n");

        err=pci_enable_device( dev );
	if (err)
		{
			printk(KERN_ERR "BLAB3 CPCI Driver: failed, Pci Enable Device returned err=%d\n", err);
			//return err;
		}

	printk("BLAB3 CPCI Driver: Device revison %d\n", blab_get_revision(dev));
	printk("BLAB3 CPCI Driver: Old device name: \"%s\"\n", dev->dev.kobj.name);
	strncpy((char*)(dev->dev.kobj.name),"BLAB3 cpci device",KOBJ_NAME_LEN);

	if ( (dev->irq!=0) && (request_irq(dev->irq, &blab_int_svc, SA_INTERRUPT|SA_SHIRQ, "pci_drv_template", dev)) )
	{
		printk(KERN_ERR "BLAB3 CPCI Driver: IRQ %d not free.\n", dev->irq );
		//goto cleanup_irq;
	}

	if (dev->irq)
		printk(KERN_ERR "BLAB3 CPCI Driver: IRQ %d.\n", dev->irq);
	else
		printk(KERN_ERR "BLAB3 CPCI Driver: No irq required/requested.\n");

	pcimemlen = pci_resource_len(dev, 0);
	pcimemstart = pci_resource_start(dev, 0);
	if(pcimemlen == 0)
	{
		printk(KERN_ERR "BLAB3 CPCI Driver: PCI memory was not initialized properly.\n");
		//goto cleanup_irq;
	}
	if(check_mem_region(pcimemstart, pcimemlen) != 0)
	{
		printk(KERN_ERR "BLAB3 CPCI Driver: memory region is already in use..erhmm, something is wrong here.\n");
		//goto cleanup_irq;
	}
	request_mem_region(pcimemstart, pcimemlen, "chardev");
	pcimemstartremap = ioremap(pcimemstart, pcimemlen);

	//pcimemstartnocache=ioremap_nocache(pcimemstart,pcimemlen);
	printk("Allocated region %d at: %lx with length %ld\n", 0, pcimemstart, pcimemlen);
	pci_set_master(dev);
	return (0);
	if (dev->irq!=0)
		free_irq( dev->irq, dev );
	pci_disable_device( dev );
	return (-EIO);
}

void remove(struct pci_dev *dev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */

	printk(KERN_ERR "BLAB3 CPCI Driver: remove..\n");
	if (dev->irq!=0)
		free_irq( dev->irq, dev );
	if(pcimemlen != 0)
		release_mem_region(pcimemstart, pcimemlen);
	return;
}

int blab_init_pci(void)
{
    int err;

    printk(KERN_INFO "BLAB3 CPCI Driver: initialize pci driver.\n");

    err = pci_register_driver(&pci_driver);
    if (err!=0)
    {
        printk("BLAB3 CPCI Driver: error %d in pci_register_driver\n", err);
        return err;
    }

    return 0;
}

void blab_exit_pci(void)
{
	pci_unregister_driver(&pci_driver);
}

