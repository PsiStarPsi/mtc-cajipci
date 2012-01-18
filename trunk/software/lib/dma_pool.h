/**
 * \file dma_pool.h
 * Declarations for the DMA pool.
 */
#ifndef ALTIX_DMA_POOL_HEADER
#define ALTIX_DMA_POOL_HEADER
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

#define ALTIX_DMA_POOL_INITIAL_ALLOC 8
#define ALTIX_DMA_UNIT_OFFSET (sizeof(uint32_t)*3)
#define ALTIX_DMA_UNIT_SIZE PAGE_SIZE
#define ALTIX_DMA_UNIT_USEFULL_SIZE (ALTIX_DMA_UNIT_SIZE - sizeof(uint)*3)

/**
 * This structure represents a single DMA accessible page. It is used by the DMA pool to build a scatter/gather buffer of any size.
 */
typedef struct altix_dma_unit_struct
{
	int* dma_vbase;							/**< Kernel space address for the DMA page. This address is used by the kernel to access the DMA data.*/
	dma_addr_t dma_handle;					/**< IOMMU address for the DMA page. This address is used by the CPCI DSP card to access the DMA data.*/
	struct altix_dma_unit_struct* next;		/**< A pointer to the next page in the DMA pool. If this is the last page this value is NULL. */
} altix_dma_unit;

/**
 * This structure represents DMA pool. It maintains a linked list of altix_dma_unit structures. Each DSP CPCI card managed by the altix driver maintains its own DMA pool
 */
typedef struct altix_dma_pool_struct
{
	altix_dma_unit* root;					/**< The root DMA page.*/
	uint size;								/**< Number of DMA pages.*/
} altix_dma_pool;

/**
 * Initializes the dma pool. This function will return a DMA pool if default size, which can be resized to fit any DMA transfer supported by the DSP CPCI card.
 */
altix_dma_pool* altix_dma_pool_init(struct pci_dev*);

/**
 * Cleans up the allocated pages, and deconfigures the IOMMU. This function is called when the driver is unloaded, since the dma pool is maintained throughout the lifetime of the driver.
 */
void altix_dma_pool_free(altix_dma_pool*, struct pci_dev*);

/**
 * Resize the dma pool for the next transfer in case the DMA pool is too small to accommodate it. This function should be called before every DMA read or write.
 */
dma_addr_t altix_dma_pool_alloc(struct pci_dev*, int size, altix_dma_pool*);

/**
 * Transfers DMA data from the dma pool into the userland buffer. NOTE this function takes a userland buffer, don't pass kmalloc'ed or vmalloc'ed buffer to this function!
 */
uint altix_dma_pool_read(altix_dma_pool*, int size, void __user* );

/**
 * Transfers DMA data from the userland buffer into the dma pool. NOTE this function takes a userland buffer, don't pass kmalloc'ed or vmalloc'ed buffer to this function!
 */
uint altix_dma_pool_write(altix_dma_pool*, int size, void __user* );

#endif
