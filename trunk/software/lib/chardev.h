/**
 * \file chardev.h
 * Definition file for the character device subsystem. These functions provide a well defined access from userland to kernel
 */
#ifndef ALTIX_CHARDEV_HEADER
#define ALTIX_CHARDEV_HEADER
#include "config.h"
#include "altix_userland.h"
#include <linux/kernel.h> /* printk() */
#include <linux/module.h>
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/tty.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#include <linux/sched.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/cred.h>
#endif

/**
 * This structure keeps track of the state of a userland file descriptor. It lives in flip->private_data, and is maintained through the life of the file descriptor.
 */
typedef struct altix_chardev_node
{
	int opened_card;	/**< ID of the opened card, ALTIX_CHARDEV_NO_CARD if no card is locked.*/
	int dma_active;		/**< DMA active switch. Determines IO destination.(PCI bar, or DMA pool)	*/
	int pid;			/**< PID of the file descriptor owner. */
} altix_chardev_node;

#define ALTIX_CHARDEV_NO_CARD -1

//Initialization and Exit
/**
 * Initializes the chardev ops struct, then attempts to get a major device number,
 * and finally registers it with the operating system.
 */
extern int altix_init_chardev(void);

/**
 * Removes the character device interface. Used in unloading the driver. Note that the driver cant be unloaded while it manages file descriptors.
 */
extern void altix_exit_chardev(void);

//File operations
#endif
