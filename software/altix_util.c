#include "lib/util.h"
#include "lib/pci_lib.h"

static inline altix_pci_lib* lib_get_device_by_id(int id)
{
	int i;
	id = id >> 6;
	for(i = 0; i < pci_lib_size; i++)
	{
		if(id == pci_lib[i]->id)
			return pci_lib[i];
	}
	return NULL;
}

void altix_reg_init(altix_pci_node *node)
{
	uint32_t cacheline;
	uint32_t img_ctl;
	altix_pci_lib * device = lib_get_device_by_id(node->id);

	memcpy_fromio(&cacheline, node->config_pcimemstartremap + ALTIX_REG_CACHE_LINE, sizeof(uint32_t));
	cacheline |= 0xC;
	memcpy_toio(node->config_pcimemstartremap + ALTIX_REG_CACHE_LINE, &cacheline, sizeof(uint32_t));

	memcpy_fromio(&img_ctl, node->config_pcimemstartremap + ALTIX_REG_ADDR_W_IMG_CTRL1, sizeof(uint32_t));
	img_ctl |= 0x3;
	memcpy_toio(node->config_pcimemstartremap + ALTIX_REG_ADDR_W_IMG_CTRL1, &img_ctl, sizeof(uint32_t));

	memcpy_fromio(&img_ctl, node->config_pcimemstartremap + ALTIX_REG_ADDR_W_IMG_CTRL2, sizeof(uint32_t));
	img_ctl |= 0x3;
	memcpy_toio(node->config_pcimemstartremap + ALTIX_REG_ADDR_W_IMG_CTRL2, &img_ctl, sizeof(uint32_t));

	if(device != NULL && device->init !=NULL)
		device->init(node);
}

void altix_reg_remove(altix_pci_node *node)
{
	altix_pci_lib * device = lib_get_device_by_id(node->id);
	if(device != NULL && device->remove !=NULL)
		device->remove(node);
}

void  altix_reg_enable_irq(altix_pci_node * node)
{
	altix_pci_lib* device;
	unsigned int data = ALTIX_REG_ENABLE_ICR;
	memcpy_toio(node->config_pcimemstartremap+ALTIX_REG_ADDR_ICR, &data, sizeof(int));
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->enable_irq !=NULL)
		device->enable_irq(node);
}

void  altix_reg_disable_irq(altix_pci_node * node)
{
	altix_pci_lib* device;
	unsigned int data = ALTIX_REG_DISABLE_ICR;
	memcpy_toio(node->config_pcimemstartremap+ALTIX_REG_ADDR_ICR, &data, sizeof(int));
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->disable_irq !=NULL)
		device->disable_irq(node);
}

void altix_reg_clear_irq(altix_pci_node * node, int* len)
{
	altix_pci_lib * device;
	unsigned int data = ALTIX_REG_CLEAR_ISR;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->clear_irq !=NULL)
		device->clear_irq(node, len);
	memcpy_fromio(&data, node->config_pcimemstartremap+ALTIX_REG_ADDR_INT_ACK, sizeof(int));
}

void altix_reg_check_irq(altix_pci_node * node, int *status)
{
	altix_pci_lib * device;
	memcpy_fromio(status, node->config_pcimemstartremap+ALTIX_REG_ADDR_ISR, sizeof(int));
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->check_irq !=NULL)
		device->check_irq(node, status);
}

void altix_reg_set_dma(altix_pci_node * node, uint size, int channel, directionType dir)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->set_dma !=NULL)
		device->set_dma(node, size, channel, dir);
}

void altix_reg_unset_dma(altix_pci_node * node)
{
	altix_pci_lib * device = lib_get_device_by_id(node->id);
	if(device != NULL && device->unset_dma !=NULL)
		device->unset_dma(node);
}

void altix_reg_datalen( altix_pci_node * node)
{
	altix_pci_lib * device = lib_get_device_by_id(node->id);
	if(device != NULL)
	{
		node->pcidatalen = device->bar_size;
		node->pcidataoffset = device->bar_offset;

		if ((device->bar_offset + device->bar_size) > node->pcimemlen)
		{
			printk(KERN_WARNING "altix CPCI Driver: warning device ID %d's memlen is less than specified driver's\n"
			       "        bar_offset=%d bar_size=%d memlen=%d\n",
			       node->id, device->bar_offset, device->bar_size, (int)node->pcimemlen);

			node->pcidatalen = node->pcimemlen - node->pcidataoffset;
		}
	}
	else
	{
		printk(KERN_WARNING "altix CPCI Driver: device ID %x is not in driver catalog, using defaults\n",
		       node->id);
		node->pcidatalen = node->pcimemlen;
		node->pcidataoffset = 0;
	}

}

void altix_reg_enable_chan(altix_pci_node * node)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->enable_chan !=NULL)
		device->enable_chan(node);

}

void altix_reg_disable_chan(altix_pci_node * node)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->disable_chan !=NULL)
		device->disable_chan(node);
}

void altix_reg_chan_status(altix_pci_node * node, int* status)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->chan_status !=NULL)
		device->chan_status(node, status);
}

void altix_reg_poll(altix_pci_node *node, int *mask)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->poll !=NULL)
		device->poll(node, mask);
}

void altix_reg_fancy(altix_pci_node *node, int cmd, int* ret)
{
	altix_pci_lib * device;
	device = lib_get_device_by_id(node->id);
	if(device != NULL && device->fancy !=NULL)
		device->fancy(node, cmd, ret);
}
