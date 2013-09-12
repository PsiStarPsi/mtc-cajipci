#ifndef _DUMB_CHAR_DEV
#define _DUMB_CHAR_DEV
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
#define DUMB_BUFF_SIZE  4096	//Buffer size

/*
*	Data structures
*/

struct dumb_chardev
{
	// set on device open, only one proccess can have it open at a time.
	struct semaphore * open_sem;
	struct cdev cdev;
	//Character device data, this should be connected to the DMA buffer
	void * data;
  	void * buffer;  // read buffer
  	int buffersize;
	int size;
	int pos;
};

/*
*	Function declarations
*/

//Initialization and Exit
extern int dumb_init_chardev(void);
extern void dumb_exit_chardev(void);

//File operations
loff_t dumb_chardev_lseek(struct file *file, loff_t offset, int orig);
int dumb_chardev_open(struct inode *inode,struct file *filep);
int dumb_chardev_release(struct inode *inode,struct file *filep);
ssize_t dumb_chardev_read(struct file *filep,char *buff,size_t count,loff_t *offp );
ssize_t dumb_chardev_write(struct file *filep,const char *buff,size_t count,loff_t *offp );
ssize_t dumb_chardev_buffered_read(struct file *filep,char *buff,size_t count,loff_t *offp );
ssize_t dumb_chardev_buffered_write(struct file *filep,const char *buff,size_t count,loff_t *offp );
#endif	/*chardev.h*/
