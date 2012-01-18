EDITOR = geany

all: clean lib

open:
	@echo "Opening source files"
	@$(EDITOR) *.h *.c Makefile
	
update:
	@echo "Updating sources"
	@svn update

lib:
	@echo "Building shared library"
	@gcc -fPIC -c -Wall libbeamdata.c -o libbeamdata.o
	@gcc -shared -Wl,-soname,libbeamdata.so -o libbeamdata.so   libbeamdata.o
	@echo "Cleaning up"
	@rm libbeamdata.o
	@strip libbeamdata.so

lib_debug:
	@echo "Building shared library"
	@gcc -fPIC -c -Wall -D  LIB_BEAM_DEBUG libbeamdata.c -o libbeamdatadbg.o
	@gcc -shared -Wl,-soname,libbeamdatadbg.so -o libbeamdatadbg.so   libbeamdatadbg.o
	@echo "Cleaning up"
	@rm libbeamdatadbg.o

#bin: 
#	@gcc libbeamdata.c -D ALTIX_LIB_MAIN  -D ALTIX_DEBUG -o test

clean:
	@echo "Cleaning source tree"
	@rm -rf libbeamdata.so test libbeamdatadbg.so

datagen: datagen.cpp
	g++ -o datagen datagen.cpp
