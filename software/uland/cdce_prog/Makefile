INSTALL_PATH=/usr/local/mtc/cajipci

all:
	g++ cdce_config.cpp -o cajipciCLOCK
	
clean:
	rm -rf cdce_prog

install: all
	@echo "Installing cajipci clock utility and init files to $(INSTALL_PATH)"
	@sudo cp cajipciCLOCK /usr/local/bin
	@sudo mkdir -p $(INSTALL_PATH)/config
	@sudo cp -R *.INI $(INSTALL_PATH)/config
	@sed "5 c\CAJIPCI_PATH=$(INSTALL_PATH)" cajipci_clock.sh >> cajipci_clock
	@chmod +x cajipci_clock
	@sudo mv cajipci_clock /etc/init.d/

uninstall:
	@sudo rm -rf $(INSTALL_PATH)/config
	@sudo rm -rf /usr/local/bin/cajipciCLOCK
	@sudo rm -rf /etc/init.d/cajipci_clock
