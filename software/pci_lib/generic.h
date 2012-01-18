//pyhint init generic_add_to_library
#include "../lib/pci_lib.h"
#include "../lib/pci.h"

#define XIN_OFFSET_CONTROL 0x4
//#define XIN_ENABLE_DMA 0x50000000 
#define XIN_ENABLE_DMA 0x70000000 
#define XIN_DISABLE_DMA 0x10000000

#define XIN_CHANNEL_MASK 0x00000003;

#define XIN_OFFSET_LEN (3*4)

#define XIN_OFFSET_ADDR (2*4)

#define XIN_OFFSET_TRANSFER (4*4)

#define XIN_OFFSET_RESET (22*4)

#define XIN_OFFSET_STATUS (5*4)

#define XIN_OFFSET_READ_COUNT_0 (6*4)
#define XIN_OFFSET_READ_COUNT_1 (16*4)
#define XIN_OFFSET_READ_COUNT_2 (18*4)
#define XIN_OFFSET_READ_COUNT_3 (20*4)

#define XIN_OFFSET_WRITE_COUNT_0 (8*4)
#define XIN_OFFSET_WRITE_COUNT_1 (10*4)
#define XIN_OFFSET_WRITE_COUNT_2 (12*4)
#define XIN_OFFSET_WRITE_COUNT_3 (14*4)

#define XIN_OFFSET_MON	(23*4)

#define XIN_OFFSET_PULSE	(24*4)

altix_pci_lib * generic_add_to_library(void);

void generic_init(altix_pci_node *node)
{
	return;
}

void generic_remove (altix_pci_node *node)
{
	return;	
}

void generic_check_irq(altix_pci_node * node,  int* result)
{
	return;
}

void generic_enable_irq (altix_pci_node *node)
{
	
	return;	
}

void generic_disable_irq (altix_pci_node *node)
{
	return;	
}

void generic_clear_irq (altix_pci_node *node, uint* size)
{	
	memcpy_fromio(size, node->pcimemstartremap + XIN_OFFSET_TRANSFER, sizeof(uint32_t));
	return;	
}

void generic_set_dma (altix_pci_node *node, uint len, int channel, directionType dir)
{
	uint control = XIN_ENABLE_DMA;
	// enable bits based on channel and direction
	
	if (dir == read_dir)
		control &= ~(0x08000000);    // clear 5th bit from left
	else if (dir == write_dir)
		control |= 0x08000000;
	else
		return;   // error!  this should not happen
		
	channel &= XIN_CHANNEL_MASK;   // in case channel is out of range,
                                   // make sure it doesn't corrupt other bits
	
	control |= (channel << 25);
	
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_LEN, &len, sizeof(uint32_t));
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_ADDR, &(node->dma_handle), sizeof(uint32_t));
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_CONTROL, &control, sizeof(uint32_t));
	return;	
}

void generic_unset_dma (altix_pci_node *node)
{
	uint control = XIN_DISABLE_DMA;
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_CONTROL, &control, sizeof(uint32_t));
	return;	
}

void generic_disable_channel(altix_pci_node * node)
{
	uint32_t reset;
	memcpy_fromio(&reset, node->pcimemstartremap + XIN_OFFSET_RESET, sizeof(uint32_t));
	reset |= 1<< node->channel;
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_RESET, &reset, sizeof(uint32_t));
}

void generic_enable_channel(altix_pci_node * node)
{
	uint32_t reset;
	memcpy_fromio(&reset, node->pcimemstartremap + XIN_OFFSET_RESET, sizeof(uint32_t));
	reset &= ~(1<< node->channel);
	memcpy_toio(node->pcimemstartremap+XIN_OFFSET_RESET, &reset, sizeof(uint32_t));
}

void generic_poll(altix_pci_node * node, int * mask)
{
	uint32_t count;
	uint offset;
	switch(node->channel)
	{
		case 0: offset = XIN_OFFSET_READ_COUNT_0; break;
		case 1: offset = XIN_OFFSET_READ_COUNT_1; break;
		case 2: offset = XIN_OFFSET_READ_COUNT_2; break;
		case 3: offset = XIN_OFFSET_READ_COUNT_3; break;
		default: offset = XIN_OFFSET_READ_COUNT_0; break;
	}
	memcpy_fromio(&count, node->pcimemstartremap + offset, sizeof(uint32_t));
	count &=0x0FFFFFFF;
	if(count > 0)
		*mask |= POLLIN | POLLRDNORM;
	else
		*mask &= ~(POLLIN | POLLRDNORM);
		
	switch(node->channel)
	{
		case 0: offset = XIN_OFFSET_WRITE_COUNT_0; break;
		case 1: offset = XIN_OFFSET_WRITE_COUNT_1; break;
		case 2: offset = XIN_OFFSET_WRITE_COUNT_2; break;
		case 3: offset = XIN_OFFSET_WRITE_COUNT_3; break;
		default : offset = XIN_OFFSET_WRITE_COUNT_0; break;
	}
	
	memcpy_fromio(&count, node->pcimemstartremap + offset, sizeof(uint32_t));
	count &=0x0FFFFFFF;
	if(count > 0)
		*mask |= POLLOUT | POLLWRNORM;
	else
		*mask &= ~(POLLOUT | POLLWRNORM);
	return;
}

void generic_channel_status(altix_pci_node * node, int* status)
{
	uint32_t zero = 0x0;
	memcpy_fromio(status, node->pcimemstartremap + XIN_OFFSET_STATUS, sizeof(uint32_t));
	memcpy_toio(node->pcimemstartremap + XIN_OFFSET_STATUS, &zero, sizeof(uint32_t));
	return;
}

void generic_fancy(altix_pci_node *node, int cmd, int* ret)
{
	switch(cmd)
	{
		case ALTIX_IOCTL_FANCY_CMD_MON_0:
		{
			uint32_t mon_cmd = 1;
			memcpy_toio(node->pcimemstartremap + XIN_OFFSET_MON, &mon_cmd, sizeof(uint32_t));
			break;
		}
		case ALTIX_IOCTL_FANCY_CMD_MON_1:
		{
			uint32_t mon_cmd = 1 << 1;
			memcpy_toio(node->pcimemstartremap + XIN_OFFSET_MON, &mon_cmd, sizeof(uint32_t));
			break;
		}
		case ALTIX_IOCTL_FANCY_CMD_PULSE_READ:
		{
			memcpy_fromio(ret, node->pcimemstartremap + XIN_OFFSET_PULSE, sizeof(uint32_t));
		}
		default:
			break;
	}
	return;
}

 altix_pci_lib *generic_add_to_library()
 {
		altix_pci_lib* new_node = (altix_pci_lib*)kmalloc(sizeof(altix_pci_lib), GFP_KERNEL);
		new_node->id = 0 >> 6;
		new_node->bar_offset = 23*sizeof(uint32_t);
		new_node->bar_size = 0x1000 - new_node->bar_offset;
		new_node->name = "Xin's DMA core.";
		new_node->init = generic_init;
		new_node->remove = generic_remove;
		new_node->enable_irq = generic_enable_irq;
		new_node->disable_irq = generic_disable_irq;
		new_node->clear_irq = generic_clear_irq;
		new_node->set_dma = generic_set_dma;
		new_node->unset_dma = generic_unset_dma;
		new_node->check_irq = generic_check_irq;
		new_node->poll = generic_poll;
		new_node->enable_chan = generic_enable_channel;
		new_node->disable_chan = generic_disable_channel;
		new_node->chan_status = generic_channel_status;
		new_node->fancy = generic_fancy;
		return new_node;
 }
