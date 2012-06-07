#ifndef _BLAB_CHAR_DEV
#define _BLAB_CHAR_DEV
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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#include <linux/sched.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/cred.h>
#endif
#define BLAB_BUFF_SIZE  4096	//Buffer size
#define IOCTL_SET_MSG _IOR(0x82, 0, int32_t)	//Generics
#define IOCTL_GET_MSG _IOR(0x82, 1, int32_t)
#define IOCTL_GET_CHANEL_COUNT _IOR(0x82, 3, int32_t)	// Prototype for the get a count of chips and chanels

#include "settings.h"

/*
*	Data structures
*/

struct blab_chardev
{
	// set on device open, only one proccess can have it open at a time.
	struct semaphore * rw_ctl;
	struct semaphore * open_ctl;
	struct cdev cdev;
	// added for buffered reads.
	char * bp;
	int position;
};

/*
*	Function declarations
*/

//Initialization and Exit
extern int blab_init_chardev(void);
extern void blab_exit_chardev(void);

//File operations
loff_t blab_chardev_lseek(struct file *file, loff_t offset, int orig);
int blab_chardev_open(struct inode *inode,struct file *filep);
int blab_chardev_release(struct inode *inode,struct file *filep);
ssize_t blab_chardev_read(struct file *filep,char *buff,size_t count,loff_t *offp );
ssize_t blab_chardev_write(struct file *filep,const char *buff,size_t count,loff_t *offp );
int blab_chardev_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
#endif	/*chardev.h*/
