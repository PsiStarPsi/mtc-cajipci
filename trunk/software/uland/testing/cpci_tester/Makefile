

cpci_tester: test.o stdPCI.o
	g++ -o cpci_tester -L$(shell pwd) -laltix -Wl,-rpath,. test.o stdPCI.o 

test.o: test.cpp
	g++ -c test.cpp

stdPCI.o: stdPCI.cpp stdPCI.h
	g++ -c stdPCI.cpp

clean: 
	rm *.o cpci_tester
