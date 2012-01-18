all: make_lib
	g++ -c -O3 main.cpp -o main.o 
	g++ -O3 -L$(shell pwd) -Wl,-rpath,. -laltix -o emul main.o 

make_lib:
	        cd ../../libaltix; $(MAKE) lib; cp libaltix.so ../testing/frontend_emul/; cp libaltix.h ../testing/frontend_emul/; cp altix_userland.h ../testing/frontend_emul/; $(MAKE) clean;


clean:
	rm -rf *.o emul
	rm -rf libaltix.h libaltix.so altix_userland.h
