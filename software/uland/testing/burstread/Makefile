all: burstread 

burstread: tester.o stdPCI.o libaltix.so
	g++  -o burstread -L$(shell pwd) -laltix -Wl,-rpath,. tester.o stdPCI.o

libaltix.so:
	cp ../libaltix/libaltix.so ../libaltix/libaltix.h ../../lib/altix_userland.h .

tester.o: libaltix.h tester.cpp
	g++ -c tester.cpp

stdPCI.o: libaltix.h stdPCI.cpp
	g++ -c stdPCI.cpp

libaltix.h:
	cp ../libaltix/libaltix.h .

clean: 
	-rm *.o
	-rm burstread 
	-rm libaltix.h
	-rm libaltix.so


