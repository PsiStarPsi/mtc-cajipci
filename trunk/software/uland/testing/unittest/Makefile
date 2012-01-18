all: unittest

test: tester.o stdPCI.test.o
	g++ -o test tester.o stdPCI.test.o 

unittest: tester.o stdPCI.o libaltix.so
	g++  -o unittest -L$(shell pwd) -laltix -Wl,-rpath,. tester.o stdPCI.o

libaltix.so:
	cp ../libaltix/libaltix.so .

stdPCI.test.o: stdPCI.test.cpp
	g++ -c stdPCI.test.cpp

tester.o: libaltix.h tester.cpp
	g++ -c tester.cpp

stdPCI.o: libaltix.h stdPCI.cpp
	g++ -c stdPCI.cpp

libaltix.h:
	cp ../libaltix/libaltix.h .

clean: 
	-rm *.o
	-rm test
	-rm libaltix.h
	-rm unittest
	-rm libaltix.so


