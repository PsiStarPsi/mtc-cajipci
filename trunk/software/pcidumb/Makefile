obj-m += pcidumb.o
pcidumb-objs := dumbpci.o chardev.o pcidev.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
load: all
	sudo sh scripts/chardev_load.sh
