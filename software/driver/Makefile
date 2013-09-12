obj-m += cajipci.o
cajipci-objs := cajipci_main.o chardev.o pcidev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
load: all
	sudo sh scripts/chardev_load.sh
	
unload:
	sudo sh scripts/chardev_unload.sh

