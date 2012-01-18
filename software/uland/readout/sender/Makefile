all: clean make_lib
	gcc -c main.c -o main.o
	gcc -Wl,-rpath,. -L. -laltix -o sender main.o
clean:
	rm -rf main.o sender
	rm -rf libaltix.h libaltix.so altix_userland.h

make_lib:
	cd ../../libaltix; $(MAKE) lib; cp libaltix.so ../readout/sender/; cp libaltix.h ../readout/sender/; cp altix_userland.h ../readout/sender/; $(MAKE) clean;

