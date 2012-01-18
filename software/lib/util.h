/**
 * \file util.h
 * This file provides PCI functions common across all OpenCores devices. It also provides the glue for interacting with the PCI device library. \see pci_lib.h
 */
#ifndef ALTIX_UTIL_HEADER
#define ALTIX_UTIL_HEADER
#include "config.h"
#include "pci.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/kobject.h>
#include <linux/cdev.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/msr.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

///Open cores registers Interrupt control
#define ALTIX_REG_ADDR_ICR 0x1EC
///Open cores registers Interrupt status
#define ALTIX_REG_ADDR_ISR 0x1F0
///Open cores registers Interrupt acknowledge
#define ALTIX_REG_ADDR_INT_ACK 0x1E8
///Open cores chache line register address:
#define ALTIX_REG_CACHE_LINE 0xC
#define ALTIX_REG_ADDR_W_IMG_CTRL1        0x184
#define ALTIX_REG_ADDR_W_IMG_CTRL2		0x194
///Open cores registers enable IRQ value
#define ALTIX_REG_ENABLE_ICR 0x1
///Open cores registers disable IRQ value
#define ALTIX_REG_DISABLE_ICR 0
///Open cores registers IRQ ACK value
#define ALTIX_REG_CLEAR_ISR 0

/**
 * This function enables interrupts on the card and calls the device specific function irq_enable if it exists.
 */
 void  altix_reg_enable_irq(altix_pci_node *);

/**
 * This function disables interrupts on the card and calls the device specific function irq_disable if it exists.
 */
 void  altix_reg_disable_irq(altix_pci_node *);

/**
 * This function generates an interrupts acknowledge on the card and calls the device specific function clear_irq if it exists.
 */
 void altix_reg_clear_irq(altix_pci_node *, int* len);

/**
 * This function checks if interrupts originated from the card and calls the device specific function check_irq if it exists.
 */
 void altix_reg_check_irq(altix_pci_node *, int* status);

/**
 * This function sets up the DMA transfer and  calls device specific set_dma if it exists.
 */
 void altix_reg_set_dma(altix_pci_node *, uint len, int chan, directionType dir);

/**
 * This function stops the DMA transfer and  calls device specific unset_dma if it exists.
 */
 void altix_reg_unset_dma(altix_pci_node *);

/**
 * This function initializes the PCI registers and calls device specific init if it exists.
 */
 void altix_reg_init(altix_pci_node *);

/**
 * This function prepares the card to be orphaned by the driver and calls device specific remove if it exists.
 */
 void altix_reg_remove(altix_pci_node *);

/**
 * This function sets the internal PCI bar size variable based on the PCI Device Library. If the device is not in the library all of the memory bar will be accessible.
 */
 void altix_reg_datalen(altix_pci_node *);

/**
 * This function polls the selected channel via the PCI Device Library. If the device is not in the library this function does nothing.
 */
 void altix_reg_poll(altix_pci_node *, int *mask);

/**
 * This function calls device specific enable_chan, in order to enable the selected channel. If the device is not in the library this function does nothing.
 */
 void altix_reg_enable_chan(altix_pci_node *);

/**
 * This function calls device specific disable_chan, in order to disable the selected channel. If the device is not in the library this function does nothing.
 */
 void altix_reg_disable_chan(altix_pci_node *);

/**
 * This function calls device specific chan_status, to check the status of the Aurora links on the card as well as the status of the last transfer. If the device is not in the library this function sets the return to 0.
 */
 void altix_reg_chan_status(altix_pci_node *, int*);

/**
 * This function calls a device specific fancy function, to perform operations not directly related to data acquisition, such as sending pulses on the monitor header.
 */
void altix_reg_fancy(altix_pci_node *, int, int*);
#endif
