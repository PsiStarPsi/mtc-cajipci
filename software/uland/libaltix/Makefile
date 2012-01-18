EDITOR = geany

all: clean lib

cp_headers:
	@echo "Copying userland header."
	@cp  ../../lib/altix_userland.h .

open:
	@echo "Opening source files"
	@$(EDITOR) *.h *.c Makefile &
	
update:
	@echo "Updating sources"
	@svn update

lib: clean cp_headers
	@echo "Building shared library"
	@gcc -fPIC -c libaltix.c -o libaltix.o
	@gcc -shared -Wl,-soname,libaltix.so -o libaltix.so   libaltix.o
	@echo "Cleaning up"
	@rm libaltix.o
	@strip libaltix.so

lib_debug: clean cp_headers
	@echo "Building shared library"
	@gcc -g -fPIC -c -D ALTIX_DEBUG libaltix.c -o libaltixdbg.o
	@gcc -g -shared -Wl,-soname,libaltixdbg.so -o libaltixdbg.so   libaltixdbg.o
	@echo "Cleaning up"
	@rm libaltixdbg.o

docs:
	@doxygen doxyfile  > /dev/null
	cd doc/latex; $(MAKE) >/dev/null; mv refman.pdf ../libaltix.pdf;cd ..; rm -rf latex
clean:
	@echo "Cleaning source tree"
	@rm -rf libaltix.so test altix_userland.h out.hex libaltixdbg.so
