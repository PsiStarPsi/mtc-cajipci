/**
 * \file pci_lib.h
 * Declarations for the PCI device library subsystem. This subsystem is used for supporting multiple revisions of firmware for CPCI DSP board.
 * Each card ID can potentially have its own set of library functions. See pci_lib/generic.h.
 */

#ifndef ALTIX_PCI_LIB_HEADER
#define ALTIX_PCI_LIB_HEADER
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
#include <linux/list.h>
#include <asm/atomic.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include "altix_userland.h"
#include "pci.h"

/**
 * PCI library structure. This structure maintains a set of pointer to device specific functions. Non device specific functions ie OpenCores pci core functions are managed by the altix_reg_* in util.h functions. \see util.h
 */
typedef struct altix_pci_lib
{
	uint id;																/**< ID of the library device. This gets matched to the upper 28 bits of the card ID.*/
	uint bar_offset;														/**< Offset for the device specific register. This is used to protect them from userspace. */
	uint bar_size;															/**< Size of the userspace accessible PCI bar size. */
	char * name;															/**< Null terminated string, which contains the name for the device. */
	void (*init)(altix_pci_node *);											/**< Initialization function.*/
	void (*remove)(altix_pci_node *);										/**< Deinitialization function. */
	void (*enable_irq)(altix_pci_node *);									/**< Device specific enable interrupt. */
	void (*disable_irq)(altix_pci_node *);									/**< Device specific disable interrupt. */
	void (*clear_irq)(altix_pci_node *, uint* size);						/**< Device specific interrupt acknowledge. Must set the size of the last transfer.*/
	void (*check_irq)(altix_pci_node *,  int* result);						/**< Device specific check if interrupt originated from this device. */
	void (*set_dma)(altix_pci_node *, uint len, int chan, directionType);	/**< Device specific start DMA transfer.*/
	void (*unset_dma)(altix_pci_node *);									/**< Device specific stop DMA transfer. */
	void (*poll)(altix_pci_node *, int* mask);								/**< Device specific poll for read and write data. */
	void (*enable_chan)(altix_pci_node *);									/**< Device specific enable one of the board channels. */
	void (*disable_chan)(altix_pci_node *);									/**< Device specific disable one of the board channels. */
	void (*chan_status)(altix_pci_node *, int*);							/**< Device specific read aurora link status. */
	void (*fancy)(altix_pci_node *, int cmd, int* ret)	;					/**< Device specific fancy command */
} altix_pci_lib;

/**
 * pci_lib.c get generated from the files in pci_lib/.h every time driver is compiled. pci_lib variable gets instantiated in the pci_lib.c. It represents an array of altix_pci_lib structures.
 */
extern altix_pci_lib **pci_lib;

/**
 * pci_lib.c get generated from the files in pci_lib/.h every time driver is compiled. pci_lib_size variable gets instantiated in the pci_lib.c It represents a number of items in the library.
 */
extern int pci_lib_size;

/**
 *  pci_lib.c get generated from the files in pci_lib/.h every time driver is compiled. This function is defined in pci_lib.c. It initializes the PCI library.
 */
void altix_pci_lib_init(void);

/**
 *  pci_lib.c get generated from the files in pci_lib/.h every time driver is compiled. This function is defined in pci_lib.c. It deinitializes the PCI library.
 */
void altix_pci_lib_free(void);

#endif
