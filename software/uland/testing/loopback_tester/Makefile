
tester: tester.o stdPCI.o libaltix.so
	g++ -g -o tester -L$(shell pwd) -laltix -Wl,-rpath,. tester.o stdPCI.o

tester.o: tester.cpp libaltix.h
	g++ -g -c tester.cpp

stdPCI.o: stdPCI.cpp libaltix.h
	g++ -g -c stdPCI.cpp

libaltix.so:
	cp ../libaltix/libaltix.so .

libaltix.h:
	cp ../libaltix/libaltix.h ../libaltix/altix_userland.h .

clean: 
	-rm *.o
	-rm tester
	-rm libaltix.h
	-rm altix_userland.h
	-rm libaltix.so


