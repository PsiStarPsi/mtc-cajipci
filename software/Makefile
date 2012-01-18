EXTRA_CFLAGS := -O2
obj-m += altix.o
altix-objs := altix_main.o altix_util.o altix_pci.o altix_chardev.o altix_dma_pool.o pci_lib.o 
EDITOR=geany

all: clean driver help

pci_library:
	@echo "Generating Device library."
	@python scripts/make_lib.py

driver: pci_library
	@echo "Building Driver."
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules >> buildlog.txt
	
clean:
	@echo "Cleaning source tree."
	@make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean >> /dev/null
	@rm -f doc/*.pdf
	@rm -rf pci_lib.c buildlog.txt

docs:
	@echo "Creating LATEX documentation in doc/"
	@echo "Documentation requires LATEX and graphviz"
	@doxygen doxyfile  > /dev/null
	cd doc/latex; $(MAKE) >/dev/null; mv refman.pdf ../altix_driver.pdf;cd ..; rm -rf latex
	cd doc/manual; latex -halt-on-error -output-format=pdf dspcpci_doc.tex; mv dspcpci_doc.pdf ../manual.pdf

update:
	@echo "Updating from subversion"
	@svn update

open:
	@echo "Opening source files"
	$(EDITOR) *.c lib/*.h Makefile  pci_lib/*.h&

load: driver
	@echo "Loading the driver"
	@ sudo bash scripts/chardev_load.sh

unload:
	@echo "Unloading the driver"
	@ sudo bash scripts/chardev_unload.sh

help:
	@echo "All done. The driver is altix.ko"
	@echo "Don't load the driver manually unless you know what you are doing."
	@echo "Use 'make load', and 'make unload', or scripts in the 'scripts/' directory"
	@echo "Note scripts have to be run from this ($(PWD)) directory."
	@echo "For complete documentation run 'make docs'"
	@echo "Documentation requires LATEX and graphviz"
