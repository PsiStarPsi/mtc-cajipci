obj-m += cajipci.o
cajipci-objs := cajipci_main.o chardev.o pcidev.o

INSTALL_PATH=/usr/local/mtc/cajipci


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
load: all
	sudo sh scripts/chardev_load.sh
	
unload:
	sudo sh scripts/chardev_unload.sh

install: all
	@echo "Installing cajipci driver and init files to $(INSTALL_PATH)"
	@sudo mkdir -p $(INSTALL_PATH)/scripts
	@sudo cp -R scripts/chardev_*.sh $(INSTALL_PATH)/scripts
	@sudo chmod +x $(INSTALL_PATH)/scripts/*.sh
	@sudo cp -R cajipci.ko $(INSTALL_PATH)
	@sed "5 c\CAJIPCI_PATH=$(INSTALL_PATH)" scripts/initloadaltix.sh >> cajipci
	@chmod +x cajipci
	@sudo mv cajipci /etc/init.d/

uninstall: unload
	@echo "Removing altix2 driver and init files to $(INSTALL_PATH)"
	sudo rm -rf $(INSTALL_PATH)
	sudo rm -rf /etc/init.d/cajipci
