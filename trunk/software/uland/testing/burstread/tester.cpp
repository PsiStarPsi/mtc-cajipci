#include "stdPCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

char* device_name;   // note: this will point to an argv c-string
					 // or a stack variable from main()
					 // e.g. DO NOT FREE THIS VARIABLE

#define BUFFER_SIZE 3000

stdPCI pci;

void start_timer();
int stop_timer();

struct timeval start, end;

void start_timer() {
	gettimeofday(&start, NULL);	
}

int stop_timer() {
	int sec, usec;
	gettimeofday(&end, NULL);

	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;

	return sec*1000000+usec;
}

int main(int argc, char** argv) {
	char default_device_name[15] = "/dev/altixpci0";
	
	device_name = default_device_name;

	if (argc == 1) {
		printf("benchmark for firehose data\n");
		printf("syntax: %s [id] [channel]\n", argv[0]);
		return -1;
	}

	int id = atoi(argv[1]);

	int from, to;
	from = argv[2][0] - '0';

	printf("reading from card %d, channel %d\n", id, from);
		
	printf("opening %s...", device_name);
	
	if (pci.createHandles(device_name) < 0) {
		printf("open handles failed\n");
		return -1;
	}
	printf("success!\n");

	char* buffer = new char[BUFFER_SIZE];
	char* buffer2 = new char[BUFFER_SIZE];

	pci.lockCard(id);

	printf("id %d, ch%d - sending and receiving 1024 MB\n", id , from);

	start_timer();

	int total = 0;
	int blocksize = 2048;
	int target = 1048576*1024;

	// init buffer
	buffer[0] = 0x3;
	for(int i=1; i < 2048; i++)
		buffer[i] = (buffer[i-1] * 0xdeadbeef) % 256;

	int readtime = 0, writetime = 0;
	int mismatch = 0;

	pci.selectChannel(from);

	while (total < target) {
		printf("%d/%d bytes\r", total, target);

		pci.selectChannel(from);

		start_timer();
		int val2 = pci.readData(buffer2, blocksize);
		readtime += stop_timer();

		total += val2;
	}
	pci.freeHandles();
	
	printf("\ndone!\n");

	printf("%d bytes - read: %d us\n", total, readtime);
	return 0;
}


