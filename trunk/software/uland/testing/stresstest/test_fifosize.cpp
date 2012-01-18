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

	int upper_limit = 1048576;

	char* buffer = new char[upper_limit];
	char* buffer2 = new char[upper_limit];

	fpci.lockCard(8);

	int* bufint = (int*)buffer;
	for(int i=0; i < upper_limit/sizeof(int); i++)
		bufint[i] = i;

	while (0 < fpci.readData(buffer2, 2048) );

	for(int i=98428; i <= upper_limit; i += 4) {
		for(int j=0; j < upper_limit; j++)
			buffer2[j] = 0;

		for(int j=0; j < 10; j++) {
			fpci.selectChannel(2);
			fpci.sendData(buffer, i);

			fpci.selectChannel(0);
			fpci.readData(buffer2, i);

			bool flag = false;
			for(int k=0; k < i; k++)
				if (buffer[k] != buffer2[k])
					flag = true;

			if (flag)
				fprintf(stderr, "X");
			else
				fprintf(stderr, "-");
		}
		fprintf(stderr, "\tsent and received %d bytes\n", i);

	}

	delete[] buffer;
	fpci.freeHandles();

	return 0;
}
