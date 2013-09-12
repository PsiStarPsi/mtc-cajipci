#include "lib/pcidev.h"

int probe(struct pci_dev *dev, const struct pci_device_id *id);

static struct pci_device_id ids[] = {
	{ PCI_DEVICE(cajipci_VEN_ID, cajipci_DEV_ID), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

struct pci_driver pci_driver = {
	.name = "pci_cajipci",
	.id_table = ids,
	.probe = probe,
	.remove = remove,
};

unsigned long pcimemstart,pcimemlen;
void *pcimemstartremap;



int cajipci_readregion_pci(void* data, int offset, int len)
//this function reads up to the end of pci memory and then returns the number of bytes read
{
	offset=offset%pcimemlen;

	if(offset+len > pcimemlen)
		len = pcimemlen-offset;
	memcpy_fromio(data, pcimemstartremap+offset, len);
	return len;
}

int cajipci_writeregion_pci(void* data, int offset, int len)
//this function writes up to the end of pci memory and then returns the number of bytes read
{
	offset=offset%pcimemlen;

	if(offset+len > pcimemlen)
		len = pcimemlen-offset;
	memcpy_toio(pcimemstartremap+offset, data, len);

	return len;
}

static unsigned char cajipci_get_revision(struct pci_dev *dev)
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

	printk(KERN_ERR "cajipci CPCI Driver: probe..\n");

        err=pci_enable_device( dev );
	if (err)
	{
		printk(KERN_ERR "cajipci CPCI Driver: failed, Pci Enable Device returned err=%d\n", err);
		return err;
	}
	printk("cajipci CPCI Driver: Device revison %d\n", cajipci_get_revision(dev));
	printk("cajipci CPCI Driver: Old device name: \"%s\"\n", dev->dev.kobj.name);
	strncpy((char*)(dev->dev.kobj.name),"cajipci cpci device",KOBJ_NAME_LEN);

	pcimemlen = pci_resource_len(dev, 1);
	pcimemstart = pci_resource_start(dev, 1);
	if(pcimemlen <= 0 || pcimemstart <= 0)
	{
		printk(KERN_ERR "cajipci CPCI Driver: PCI memory was not initialized properly.\n");
		return (-EIO);
	}
	if(check_mem_region(pcimemstart, pcimemlen) != 0)
	{
		printk(KERN_ERR "cajipci CPCI Driver: memory region is already in use..erhmm, something is wrong here.\n");
		return (-EIO);
	}
	request_mem_region(pcimemstart, pcimemlen, "chardev");
	pcimemstartremap = ioremap(pcimemstart, pcimemlen);
	printk("Allocated region %d at: %lx with length %ld\n", 1, pcimemstart, pcimemlen);
	return (0);
}

void remove(struct pci_dev *dev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */

	printk(KERN_ERR "cajipci CPCI Driver: remove..\n");
	if(pcimemlen != 0)
		release_mem_region(pcimemstart, pcimemlen);
	return;
}

int cajipci_init_pci(void)
{
    int err;

    printk(KERN_INFO "cajipci CPCI Driver: initialize pci driver.\n");

    err = pci_register_driver(&pci_driver);
    if (err!=0) {
        printk("cajipci CPCI Driver: error %d in pci_register_driver\n", err);
        return err;
    }

    return 0;
}

void cajipci_exit_pci(void)
{
	pci_unregister_driver(&pci_driver);
}

