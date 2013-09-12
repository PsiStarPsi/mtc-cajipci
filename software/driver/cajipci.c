//Required imports from the kernel tree
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
//Configuration Imports
#include "lib/about.h"
#include "lib/chardev.h"
#include "lib/pcidev.h"

static int cajipci_module_init(void)
{
	int err_pci=0,err_char=0;

	printk(KERN_ERR "Initializing cajipci CPCI driver..\n" );

	err_pci=cajipci_init_pci();
	printk(KERN_ERR "..cajipci_init_pci() == %d\n",err_pci);
	if (err_pci != 0) goto fail_pci;

	err_char=cajipci_init_chardev();
	printk(KERN_ERR "..cajipci_init_chardev() == %d\n",err_char);
	if (err_char != 0) goto fail_chardev;

	printk(KERN_ERR "Initialized cajipci CPCI driver.\n" );
	return 0;	//Initialization is compleated

	fail_chardev:
		printk(KERN_ERR "Failed to initialize cajipci char driver.\n" );
		cajipci_exit_chardev();
	fail_pci:
		printk(KERN_ERR "Failed to initialize cajipci CPCI driver, char dev was not initialized.\n" );
		cajipci_exit_pci();
		return -1;
}

static void cajipci_module_exit(void)
{
	//Remove the character device
	printk(KERN_WARNING "exit cajipci CPCI driver.\nexit cajipci char driver.\n" );
	cajipci_exit_chardev();
	cajipci_exit_pci();
}
module_init(cajipci_module_init);
module_exit(cajipci_module_exit);

