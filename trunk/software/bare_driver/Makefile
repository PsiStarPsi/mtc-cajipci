obj-m += pcibare.o
pcidrv-objs := pcibare.o chardev.o pcidev.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
