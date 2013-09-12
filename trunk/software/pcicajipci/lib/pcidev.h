#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>
#include <linux/kobject.h>
#include <linux/cdev.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/msr.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26)
#include <linux/sched.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
#include <linux/cred.h>
#endif

#if !defined(KOBJ_NAME_LEN)
#define KOBJ_NAME_LEN 20
#endif

#if !defined(SA_SHIRQ)
#define SA_SHIRQ IRQF_SHARED
#endif

#if !defined(SA_INTERRUPT)
#define SA_INTERRUPT IRQF_DISABLED
#endif

#define cajipci_VEN_ID 0x10EE
#define cajipci_DEV_ID 0x0002


extern int cajipci_init_pci(void);
extern void cajipci_exit_pci(void);

int cajipci_readregion_pci(void* data,int offset, int len);

int cajipci_writeregion_pci(void* data,int offset, int len);

int probe(struct pci_dev *dev, const struct pci_device_id *id);
void remove(struct pci_dev *dev);


