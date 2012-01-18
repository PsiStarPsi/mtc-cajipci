CFLAGS=-L$(shell pwd) -laltix -Wl,-rpath,.

all: stress_block stress_fifo

stress_block: test_blocksize.o stdPCI.o libaltix.so
	g++ -o stress_block $(CFLAGS) test_blocksize.o stdPCI.o

stress_fifo: test_fifosize.o stdPCI.o libaltix.so
	g++ -o stress_fifo $(CFLAGS) test_fifosize.o stdPCI.o


fifotest: test_fifo.o stdPCI.o libaltix.so
	g++ -o fifotest $(CFLAGS) test_fifo.o stdPCI.o

test_fifo.o: test_fifo.cpp libaltix.h
	g++ -c test_fifo.cpp


test_blocksize.o: test_blocksize.cpp libaltix.h
	g++ -c test_blocksize.cpp

test_fifosize.o: test_fifosize.cpp libaltix.h
	g++ -c test_fifosize.cpp

stdPCI.o: libaltix.h stdPCI.cpp
	g++ -c stdPCI.cpp

libaltix.h:
	cp ../libaltix/libaltix.h .

libaltix.so:
	cp ../libaltix/libaltix.so .

clean: 
	-rm *.o stress_* libaltix.h libaltix.so
