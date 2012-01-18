/**
 * \file pci.h
 *  Declarations for the internal pci datastructures and functions.
 */
#ifndef ALTIX_PCI_HEADER
#define ALTIX_PCI_HEADER
#include "config.h"
#include "altix_userland.h"
#include "dma_pool.h"
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
#include <linux/poll.h>
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#include <linux/sched.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/cred.h>
#endif

#if !defined(KOBJ_NAME_LEN)
///Make old kernels happy
#define KOBJ_NAME_LEN 20
#endif

#if !defined(SA_SHIRQ)
///Shared interupts current setup does not support dedicated interupts
#define SA_SHIRQ IRQF_SHARED
#endif

#if !defined(SA_INTERRUPT)
#define SA_INTERRUPT IRQF_DISABLED
#endif

/**
 * Node in the internal doubly linked list. Represents a hardware device.
 */
typedef struct altix_pci_node
{
	unsigned long pcimemstart; 			/**< START of the PCI bar in IO space.*/

	void *pcimemstartremap; 			/**< START of the PCI bar in KERNEL space.*/
	unsigned long pcimemlen; 			/**< PCI bar length.*/
	unsigned long pcidatalen;  			/**< length of bar that is actual data.*/
	unsigned long pcidataoffset;  		/**< offset of data in data bar. */

	void *config_pcimemstartremap; 		/**< START of the PCI config bar in KERNEL space.*/
	unsigned long config_pcimemstart; 	/**< START of the PCI config bar in IO space.*/
	unsigned long config_pcimemlen; 	/**< PCI config bar length.*/

	uint id; 							/**< Card ID to be read from the card.*/
	pid_t pid; 							/**< PID of the process which is locking this card.*/

	unsigned long node_lock; 			/**< bitfield for locking the node to a PID.*/
	struct semaphore * io_lock; 		/**< Lock for IO.*/
	wait_queue_head_t * rd_waitq; 		/**< Wait queue for DMA.*/

	unsigned int num_reads;				/**< Number of reads which occurred on this card.*/
	unsigned int num_writes; 			/**< Number of writes which occurred on this card.*/
	uint64_t bytes_read;				/**< Number of bytes read.*/
	uint64_t  bytes_written;			/**< Number of bytes written.*/

	altix_dma_pool * dma_pool;			/**< DMA pool data structure. */
	int irq_enabled;					/**< Flag for the interrupt enable. */
	int irq_flag;						/**< Used in the DMA wait queue. */
	dma_addr_t dma_handle;				/**< Bus DMA address. */
	int channel;						/**< Selected channel. */
	struct pci_dev	*dev;  				/**< Device Handle link. */

	struct altix_pci_node* next; 		/**< Next item in doubly linked list. NULL if last item. */
	struct altix_pci_node* prev; 		/**< Previous item in a doubly linked list. NULL if first item.*/
} altix_pci_node;

/**
 * Doubly Linked list of altix_pci_node.
 * Can be locked for manipulation.
 */
typedef struct
{
	int length; 						/**< Number of PCI cards in the list.*/
	altix_pci_node * root; 				/**< Root node of the list*/
	struct semaphore * probe_lock; 		/**< Global PCI side lock.*/
} altix_pci_head;

/**
 * Direction enum for sanity
 */
typedef enum  {read_dir, write_dir} directionType;

//Internals
/**
 *	Probe Function runs on start ups for each PCI device. Probe function will set up the PCI device and map its memory into the kernel space. Due to race condition, multiple devices are set up sequentially, on first come first serve basis. PCI device gets renamed to ALTIX PCI #{DEVICE NUMBER} in the proc directory.
 */
int probe(struct pci_dev *dev, const struct pci_device_id *id);

/**
 * Orphans the device. Disables IRQ, unmaps device memory, and removes device from kernel datastructures.
 */
void remove(struct pci_dev *dev);

/**
 * PCI subsystem exit function. Frees PCI data structures and unregisters the device driver.
 */
extern void altix_exit_pci(void);

/**
 * PCI subsystem initialization function. Sets up kernel PCI data structures and registers the device driver.
 */
extern int altix_init_pci(void);

//Getting card information
/**
 * Returns the number of cards manaed by this driver.
 */
int altix_pci_get_cards_number(void);

/**
 * Returns an array of card description
 */
altix_pci_card_info* altix_pci_get_cards(void);

/**
 * Returns the memory length of a card selected by ID. Returns 0 if the id is not present.
 */
unsigned long altix_pci_get_memlen_by_id(int id);

//Locking Cards

///Returned by locking and IO functions, signifies success.
#define ALTIX_PCI_CARD_OK 0

///Returned by locking and IO functions, card is already locked.
#define ALTIX_PCI_CARD_BUSY -1

///Returned by locking and IO functions, card is not plugged in, or did not initialize properly.
#define ALTIX_PCI_CARD_UNAVAILABLE -2

///Returned by locking and IO functions, system call was interrupted while waiting on its critical section.
#define ALTIX_PCI_CARD_IO_INTERRUPTED -3

/**
 * Lock the card based on its unique ID. Returns ALTIX_PCI_CARD_UNAVAILABLE if id does not match any of the managed cards and ALTIX_PCI_CARD_BUSY if card is already locked.
 */
int altix_pci_lock_by_id(int id);

/**
 * Release the card based on its unique ID. Returns ALTIX_PCI_CARD_BUSY if card is not locked by the requesting PID
 */
int altix_pci_release_by_id(int id);

/**
 * Release the card based on its unique ID. Only called on close of a file descriptor to free the card for future use. Returns ALTIX_PCI_CARD_UNAVAILABLE if id does not match any of the managed cards.
 */
int altix_pci_unconditional_release_by_id(int id);

//Reading and writing. Requires a lock.
/**
 * IO read from a pci card selected by its ID. Requires a lock by the PID. Returns number of bytes read from the PCI bar, or a negative error code.
 */
int altix_pci_readregion_by_id(void* data, int offset, int len, int id);

/**
 * IO write to a pci card selected by its ID. Requires a lock by the PID. Returns number of bytes written from the PCI bar, or a negative error code.
 */
int altix_pci_writeregion_by_id(void* data, int offset, int len, int id);

/**
 * Performs a DMA read on card - requires process to hold lock on card. Returns number of bytes read from the dma pool, or a negative error code. Will block while waiting for the interrupt.
 */
int altix_pci_dma_read_by_id(void __user* data,  int len, int id);

/**
 * Performs a DMA write on card - requires process to hold lock on card. Returns number of bytes written from the dma pool, or a negative error code. Will block while waiting for the interrupt.
 */
int altix_pci_dma_write_by_id(void __user* data,  int len, int id);

/**
 * Interrupt disable. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_disable_irq_by_id(int id);

/**
 * Interrupt enable. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_enable_irq_by_id(int id);

/**
 * Select DMA channel. There are 4 channels per board. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_set_channel(int id, int chan);

/**
 * Enable DMA channel. There are 4 channels per board. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_enable_channel(int id);

/**
 * Enable DMA channel. There are 4 channels per board. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_disable_channel(int id);

/**
 * Get status of the last transfer. Requesting PID must maintain the lock on the card. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_channel_status(int id, int * status);

/**
 * Returns the performance stats for the pci cards, Return an array of altix_pci_card_stat structures. All the data is maintained in the kernel instead of the firmware for fast access.
 */
altix_pci_card_stat * altix_pci_get_card_stats(void);

/**
 * Poll the selected channel on the selected card. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
unsigned int altix_pci_poll(struct file* filp, unsigned int card_id, poll_table *wait);

/**
 * Calls a fancy function on the device. Requesting PID must maintain a lock on the device. Returns a negative error code on failier or ALTIX_PCI_CARD_OK on success.
 */
int altix_pci_fancy(int id, int cmd, int* ret);
#endif
