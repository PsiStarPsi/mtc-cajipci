#include "lib/chardev.h"
#include "lib/pcidev.h"
#include <asm/div64.h>

extern struct blab_buffered_data* blab_data;

struct blab_chardev * chardev;

struct file_operations blab_chardev_fops;

int blab_chardev_major=-1;


#define READBUFFERSIZE 4096

int blab_init_chardev()
{
	dev_t tmpchrdev;
	chardev = kmalloc(sizeof(struct blab_chardev), GFP_KERNEL);
	chardev->rw_ctl = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
	chardev->open_ctl = kmalloc(sizeof(struct semaphore), GFP_KERNEL);

	sema_init(chardev->open_ctl, 1);
	sema_init(chardev->rw_ctl, 1);

	chardev->bp=NULL;
	chardev->position=0;

	blab_chardev_fops.open =  blab_chardev_open;
	blab_chardev_fops.read = blab_chardev_read;
	blab_chardev_fops.write = blab_chardev_write;
	blab_chardev_fops.release = blab_chardev_release;
	blab_chardev_fops.ioctl = blab_chardev_ioctl;
	if(alloc_chrdev_region (&tmpchrdev,0,1,"chardev")<0)
	{
		printk("BLAB3 CPCI Driver: couldnt get major/minor number.\n");
		return -1;
	}
	blab_chardev_major=MAJOR(tmpchrdev);
	cdev_init ( &chardev->cdev, &blab_chardev_fops);
	chardev->cdev.owner = THIS_MODULE;
	chardev->cdev.ops = &blab_chardev_fops;
	if(cdev_add(&chardev->cdev, MKDEV(blab_chardev_major, 0), 1) < 0)
	{
		printk("BLAB3 CPCI Driver: Failed to initialize character device\n");
		return -1;
	}
	printk("BLAB3 CPCI Driver: Initialized character device with major number %d\n", blab_chardev_major);

	return 0;
}

void blab_exit_chardev()
{
	cdev_del(&chardev->cdev);
	unregister_chrdev_region (MKDEV(blab_chardev_major, 0),1);
	kfree(chardev->open_ctl);
	kfree(chardev->rw_ctl);
	kfree(chardev);
}

int blab_chardev_open(struct inode *inode,struct file *filep)
{
	down(chardev->open_ctl);
	printk(KERN_INFO "BLAB3 CPCI Driver: %d opened the pci driver' chr dev\n", current->pid);
	blab_data->r_pos = 0;
	blab_data->w_pos = 1;
	blab_data->stamp = 1;
	sema_init(blab_data->rw_sem, 1);
	memset(blab_data->data[0], 0xFF,sizeof(int32_t)*BUFFSIZE);
	return 0;
}

int blab_chardev_release(struct inode *inode,struct file *filep)
{
	up(chardev->open_ctl);
	blab_data->stamp = 0;
	blab_data->stamp = 0;
	kfree(blab_data->rw_sem);
	printk(KERN_INFO "BLAB3 CPCI Driver: %d close the pci driver's chr dev\n", current->pid);
	return 0;
}



ssize_t blab_chardev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int stamp;
	//allow only one read at a time.
	if(count != BUFFSIZE*4 +4)
	{
		printk(KERN_WARNING "BLAB3 CPCI Driver: %d tried to read the %d but the block size is %d\n", current->pid, (int)count, BUFFSIZE*4+4);
		return 0;
	}
	//not too clever, since its not atomic. should maybe be rewriten....
	if (down_interruptible (chardev->rw_ctl))
	{
		printk(KERN_WARNING "BLAB3 CPCI Driver: %d tried to read the blabpci driver but its already in use\n", current->pid);
		return -ERESTARTSYS;
	}
	down(blab_data->rw_sem);
	stamp = blab_data->data_stamp[blab_data->r_pos];
	copy_to_user(buf, blab_data->data[blab_data->r_pos], BUFFSIZE*4);
	copy_to_user(buf + BUFFSIZE*4, &stamp, sizeof(int));
	blab_data->r_pos = (blab_data->r_pos+1)%BUFFNUM;
	up(chardev->rw_ctl);
	return BUFFSIZE*4 + 4;
}

ssize_t blab_chardev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	//  This function is left for card control.
	//Since the protocol is not there yet, it is left blank
	if (down_interruptible (chardev->rw_ctl))
	{
		printk(KERN_WARNING "BLAB3 CPCI Driver: %d tried to read the blabpci driver but its already in use\n", current->pid);
		return -ERESTARTSYS;
	}
	down(blab_data->rw_sem);

	up(chardev->rw_ctl);
	return 0;
}


int blab_chardev_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int32_t temp;
	temp =  ioctl_param;
	switch (ioctl_num) {
	case IOCTL_SET_MSG:
		blab_writeregion_pci((void*) &temp, 0,sizeof(int32_t));
		return 0;
		break;

	case IOCTL_GET_MSG:
		blab_readregion_pci(&temp, 0, sizeof(int32_t));
		return temp;
		break;
	}
	return 0;
}
