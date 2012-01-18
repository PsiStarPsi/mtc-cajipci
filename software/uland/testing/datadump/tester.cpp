#include "stdPCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFFER_SIZE 65536

char* device_name;   // note: this will point to an argv c-string
					 // or a stack variable from main()
					 // e.g. DO NOT FREE THIS VARIABLE
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
	int fd = open("log.txt", O_WRONLY | O_CREAT);

	char default_device_name[15] = "/dev/altixpci0";
	device_name = default_device_name;

	if (argc == 1) {
		printf("retrieve data from channel and dump to binary file\n");
		printf("syntax: %s [id] [channel]\n", argv[0]);
		return -1;
	}

	int id = atoi(argv[1]);

	int from;
	from = argv[2][0] - '0';

	printf("reading from card %d, ch%d\n", id, from);

	printf("opening %s\n", device_name);
	
	if (pci.createHandles(device_name) < 0) {
		printf("open handles failed\n");
		return -1;
	}

	char* buffer = new char[BUFFER_SIZE];
	char* buffer2 = new char[BUFFER_SIZE];

	pci.lockCard(id);
	printf("sending and receiving 2 GB\n");

	start_timer();

	long long total = 0;
	int blocksize = 2048;
	long long target = (long long)1048576*1024*2;
	int val2;

	// init buffer
	buffer[0] = 0x3;
	for(int i=1; i < 2048; i++)
		buffer[i] = (buffer[i-1] * 0xdeadbeef) % 256;

	long long readtime = 0, writetime = 0;
	int mismatch = 0;

	while (total < target) {
		pci.selectChannel(from);

		start_timer();
		val2 = pci.readData(buffer2, blocksize);
		readtime += (long long)stop_timer();

		start_timer();
		write(fd, buffer2, val2);
		writetime += (long long)stop_timer();

		fflush(stdout);

		total += val2;
		printf("read+wrote %lld/%lld bytes\r", total, target);
	}
	pci.freeHandles();
	
	printf("done!\n");

	printf("%lld bytes - read: %lld us, logging %lld us, total %lld us\n", total, readtime, writetime, readtime + writetime);

	printf("%d mismatches\n\n", mismatch);

	return 0;
}


