#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h> 
#include "stdPCI.h"

//#define DRIVER_NAME "foobar"
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
	char device_name[100];
	
	int fd;

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
	int upper_limit = 131072;

	char* buffer = new char[upper_limit];
	char* buffer2 = new char[upper_limit];

	fpci.lockCard(33);

	// flush buffer
	fpci.selectChannel(1);
	while (0 < fpci.readData(buffer2, upper_limit) );

	for(int i=1024; i <= upper_limit; i += 1024) {
		for(int j=0; j < i; j++) {
			buffer[j] = rand() % 256;
			buffer2[i] = rand() % 256;
		}

		fpci.selectChannel(2);
		fpci.sendData(buffer, i);

		fpci.selectChannel(1);
		fpci.readData(buffer2, i);

		int mismatch = 0;

		for(int j=0; j < i; j++)
			if ((buffer[j] != buffer2[j]) ) {
				mismatch++;
			}				

		if (mismatch > 0)
			fprintf(stderr, "(%d)", mismatch);

		fprintf(stderr, "sent and received %d bytes\n", i);
	}

	delete[] buffer;
	fpci.freeHandles();

	return 0;
}
