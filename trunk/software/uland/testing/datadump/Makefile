all: datadump

datadump: tester.o stdPCI.o libaltix.so
	g++  -o datadump -L$(shell pwd) -laltix -Wl,-rpath,. tester.o stdPCI.o

libaltix.so:
	cp ../libaltix/libaltix.so .

tester.o: libaltix.h tester.cpp
	g++ -c tester.cpp

stdPCI.o: libaltix.h stdPCI.cpp
	g++ -c stdPCI.cpp

libaltix.h:
	cp ../libaltix/libaltix.h ../libaltix/altix_userland.h .

clean: 
	-rm *.o
	-rm datadump
	-rm libaltix.h
	-rm altix_userland.h
	-rm libaltix.so


