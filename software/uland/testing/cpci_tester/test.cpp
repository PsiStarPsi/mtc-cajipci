#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h> 
#include "stdPCI.h"

#define DRIVER_NAME "/dev/altixpci0"

#define WAIT_TIME 1
#define MAX_BUFFERSIZE 0x40000
#define SPEEDTEST_TARGETSIZE 1048576


stdPCI fpci;

struct timeval start, end;

void start_timer() {
	gettimeofday(&start, NULL);	
}

int stop_timer() {
	int sec, usec;
	gettimeofday(&end, NULL);

	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;

	return sec*1000+usec/1000;
}

int main(int argc, char* argv[]) {
	char buffer[MAX_BUFFERSIZE];
	char device_name[100];
	int hit, miss, fd, fpos, current = 0;
	
	if (argc < 2) {
		fd = fpci.createHandles(DRIVER_NAME) ;
		strcpy(device_name, DRIVER_NAME);
	}
	else {
		fd = fpci.createHandles(argv[1]);
		strcpy(device_name, argv[1]);
	}

	if (fd == -1) {
		printf("open device failed\n\nsyntax: %s [device]\n", argv[0]);
		return -1;
	}
	printf("opened PCI card handle\n");

	fpos = 0;
	int ch = -1;
	int id = -1;
	fpci.setPosition(fpos);

	while (buffer[0] != 'q') {
		printf("%s[%d]:%d> ", device_name, id, ch );
		fgets(buffer, MAX_BUFFERSIZE, stdin);
		
		// remove newline that fgets generates
		buffer[strlen(buffer)-1] = '\0';
		
		if (buffer[0] == 'r') {  // read
			int readSize, size = atoi(&buffer[1]);	
			readSize = fpci.readData(buffer, size);

			printf("read %d/%d bytes\n", readSize, size);

			for(int i=0; i < readSize; i+=4)
				printf("%08x ", *(unsigned int*) &buffer[i]);
			
			printf("\n");

			fpos += readSize;
		}
		else if (buffer[0] == 'w') { // write
			int size = strlen(buffer) - 2;
			if (size < 0)
				continue;

			fpci.sendData(&buffer[2], size);
			fpos += size;
			printf("written %d bytes\n", size);
		}
		else if (buffer[0] == 'c') {
			int result, chan;
			chan = atoi(buffer+1);
			result = fpci.selectChannel( chan );
			printf("selecting channel %d, result=%d\n", chan, result);
			if (result == 0)
				ch = atoi(buffer+1);
		}		
		else if (buffer[0] == 'l') {
			int result, num = atoi(&buffer[1]);
			printf("attempting to lock card %d... ", num);
			result = fpci.lockCard(num);
			printf("lockcard returned %d\n", result);
			if (result == 0) 
				id = num;
		}

		else if (buffer[0] == 'u') {
			int result, num = atoi(&buffer[1]);
			result = fpci.unlockCard(num);
			printf("unlockcard returned %d\n", result);
			if (result == 0) {
				id = -1;
				ch = -1;
			}
		}
		else if (buffer[0] == 'i') {
			printf("number of cards attached: %d\n", fpci.getCardCount() );
			vector<int> ids = fpci.getIDList();
			for(int i=0; i < fpci.getCardCount(); i++) {
				printf("card #%d: id=%d memlen=%d\n", i+1,
					ids[i], fpci.getMemoryLength(ids[i]) );
			}
			printf("\n");
		}

		else if (buffer[0] == '?') {
			printf("commands: (i)nfo,\n (l)ock, (u)nlock, (c)hannel \n(r)ead, (w)rite\n(q)uit\n");
		}

	}

	fpci.freeHandles();

	return 0;
}
