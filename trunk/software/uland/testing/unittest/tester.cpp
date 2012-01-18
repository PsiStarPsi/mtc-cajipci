#include "stdPCI.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define TOTAL_RANDOM 0 //1000000
#define TOTAL_CHANMODE  0 // 100 //1000
#define TOTAL_TIMED 0
#define WAIT_TIME 1

// comment once all channels are working again
#define SKIP_MULTICHAN

#define DEFAULT_CHAN 2

// uncomment to print buffers from verify tests
#define VERBOSE_LOGGING

char* device_name;   // note: this will point to an argv c-string
					 // or a stack variable from main()
					 // e.g. DO NOT FREE THIS VARIABLE
void start_timer();  // timer for running speed tests - integer is in microseconds (us)
int stop_timer();

/*
 * Guidelines for test functions:
 * 
 * - function should create their own pci object
 * - don't assume all tests will pass - if a test may fail, do
 *   appropriate cleanup if necessary so other tests don't fail
 */

void pci_info();             // information about attached cards
void pci_verify_test();      // verification tests (seq, rand, delay)
void pci_lock_test();        // lock/unlock functionality
void pci_dma_test();         // DMA verification/speed test
void pci_fork_test();        // concurrency tests
void pci_speed_test();       // speed tests of individual cards

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
	printf("test\n");

#ifdef VERBOSE_LOGGING
// this is just to empty the file
	FILE* fout = fopen("log.txt", "w");
	fclose(fout);	
	fout = fopen("log_ch.txt", "w");
	fclose(fout);	

#endif

	char default_device_name[15] = "/dev/altixpci0";
	
	if (argc > 1)
		device_name = argv[argc-1];
	else
		device_name = default_device_name;
	
	printf("opening %s\n", device_name);
	
	stdPCI pci;

	if (pci.createHandles(device_name) < 0) {
		printf("open handles failed\n");
		return -1;
	}
	
	pci.freeHandles();

	pci_info();
	pci_verify_test();

	pci_speed_test();
//	pci_lock_test();
	pci_dma_test();
	pci_fork_test();
	return 0;
}

void pci_info() {
	stdPCI pci;
	pci.createHandles(device_name);

	printf("open handles successful\n");
	printf("info about cards:\n");
	printf("%d cards total\n",  pci.getCardCount() );
	
	vector<int> ids = pci.getIDList();
	for(unsigned int i=0; i < ids.size(); i++) {
		printf("Card %d     ID: %d\n", i+1, ids[i]);
		printf("        memlen: %d\n", pci.getMemoryLength(ids[i]) );
	}
	
	pci.freeHandles();
}

int rw_test(stdPCI pci, const char* buffer, int n, char* desc = "", int waitTime = 0);  // default values
int rw_test(stdPCI pci, const char* buffer, int n, char* desc, int waitTime) {
	printf("testing %s...", desc);
	fflush(stdout);

	pci.rewind();

	pci.sendData(buffer, n);
	printf("sent...");
	fflush(stdout);

	pci.rewind();

	sleep(waitTime);

	char* tmpBuffer = new char[n];

	int result = pci.readData(tmpBuffer, n);
	printf("read back %d bytes...", result);
	fflush(stdout);

	int mismatch = 0;
	for(int i=0; i<n; i++) {

		if (buffer[i] != tmpBuffer[i])
			mismatch++;
	}

#ifdef VERBOSE_LOGGING
	FILE* fout = fopen("log.txt", "a");
	fprintf(fout, "%s - %d mismatches", desc, mismatch);
	
	fprintf(fout, "pairs: ");
	for(int i=0; i < n; i++)
		if (buffer[i] != tmpBuffer[i])
			fprintf(fout, "<%02x %02x> ", (unsigned char)buffer[i], (unsigned char)tmpBuffer[i]);
		else
			fprintf(fout, "[%02x] ", (unsigned char)buffer[i]);

	fprintf(fout, "\n\n");
	fclose(fout);

#endif

	delete[] tmpBuffer;

	printf("%d errors", mismatch);
	if ((mismatch > 0))
		printf("\n");
	else {
		printf("\r");
		fflush(stdout);
	}


	return mismatch;
}

void pci_verify_test() {
	stdPCI pci;

	printf("=== accuracy tests ===\n");
	pci.createHandles(device_name);

	for(unsigned int k=0; k < pci.getIDList().size(); k++) {
		printf("Testing card #%d (id %d)\n", k+1, pci.getIDList()[k] );

		pci.lockCard( pci.getIDList()[k] );
		printf("locked!\n");
		
		pci.selectChannel(DEFAULT_CHAN);

		int MAX_BUFFERSIZE = pci.getMemoryLength( pci.getIDList()[k] );
		MAX_BUFFERSIZE = 2080+260;
		int hit, miss;
		hit = miss = 0;
		
		unsigned char *buffer = new unsigned char[MAX_BUFFERSIZE];
		unsigned char *readoutbuffer;
		char desc[100];

		// sequential data
		hit = miss = 0;
		for(int i=0; i < 256; i++) {
			for(int j=0; j<MAX_BUFFERSIZE; j++)
				buffer[j] = i;

			sprintf(desc, "buffer filled w/%02x", i);

			int result = rw_test(pci, (char*)buffer, MAX_BUFFERSIZE, desc);		
			if (result == 0)
				hit++;
			else
				miss++;
		}
		printf("\n%d successful readouts, %d data mismatches\n\n", hit, miss);

		// random data
		hit = miss = 0;
		for(int i=0; i < TOTAL_RANDOM; i++) {
			for(int j=0; j<MAX_BUFFERSIZE; j++)
				buffer[j] = rand() % 256;

			sprintf(desc, "random data %d/%d", i, TOTAL_RANDOM);

			int result = rw_test(pci, (char*)buffer, MAX_BUFFERSIZE, desc);		
			if (result == 0)
				hit++;
			else
				miss++;
		}
		printf("\n%d successful readouts, %d data mismatches\n\n", hit, miss);

		// time-delay readout
		hit = miss = 0;
		for(int i=0; i < TOTAL_TIMED; i++) {
			for(int j=0; j<MAX_BUFFERSIZE; j++)
				buffer[j] = rand() % 256;

			sprintf(desc, "sleep case %d/%d", i, TOTAL_TIMED);

			int result = rw_test(pci, (char*)buffer, MAX_BUFFERSIZE, desc, WAIT_TIME);		
			if (result == 0)
				hit++;
			else
				miss++;
		}
		printf("\n%d successful readouts, %d data mismatches\n\n", hit, miss);

#ifndef SKIP_MULTICHAN

		hit = miss = 0;
		delete[] buffer;
		int NUM_CHAN = 4;
		buffer = new unsigned char[MAX_BUFFERSIZE * NUM_CHAN];
		readoutbuffer = new unsigned char[MAX_BUFFERSIZE * NUM_CHAN];

		printf("testing multiple channel send/receive\n");
		// four channels mode
		for(int i=0; i < TOTAL_CHANMODE; i++) {
			printf("iteration %d...", i);
			for(int j=0; j<MAX_BUFFERSIZE*NUM_CHAN; j++)
				buffer[j] = rand() % 256;

			for(int j=0; j < NUM_CHAN; j++) {
				pci.selectChannel(j);

				pci.sendData((char*)&buffer[MAX_BUFFERSIZE * j], MAX_BUFFERSIZE);
			}

			for(int j=0; j < NUM_CHAN; j++) {
				pci.selectChannel(j);

				int read = pci.readData((char*)&readoutbuffer[MAX_BUFFERSIZE * j], MAX_BUFFERSIZE);
				printf("chan %d: %d bytes read\n", j, read);
			}

			int errcount = 0;
			FILE* fout = fopen("log_ch.txt", "a");
			for(int j=0; j < MAX_BUFFERSIZE*NUM_CHAN; j++) {
				if ((j % MAX_BUFFERSIZE) == 0)
					fprintf(fout, "Channel %d: ", j/MAX_BUFFERSIZE);
				if (readoutbuffer[j] == buffer[j])
					fprintf(fout, "[%02x] ", buffer[j]);
				else {
					fprintf(fout, "<%02x %02x> ", buffer[j], readoutbuffer[j]);
					errcount++;
				}
				if (((j+1) % MAX_BUFFERSIZE) == 0) {
					fprintf(fout, "\n");
				}
			}
			
			if (errcount > 0)
				miss++;
			else
				hit++;

			fprintf(fout, "\n");
			fclose(fout);

			printf("%d errors\n", errcount);
		}
		printf("%d successful readouts, %d errors\n", hit, miss);

		delete[] buffer;
		delete[] readoutbuffer;
#endif

	}

	pci.freeHandles();

}

void pci_lock_test() {
	stdPCI pci, pci2;
	int errcode;

	pci.createHandles(device_name);
	pci2.createHandles(device_name);

	vector<int> ids = pci.getIDList();

	for(unsigned int k=0; k < ids.size(); k++) {
		int num = ids[k];
		
		//try to lock a card in use
		printf("test: try to lock a card in use\n");
		errcode = pci.lockCard(num);
		printf("[1]lockcard on %d, result: %d\n", num, errcode);
		errcode = pci2.lockCard(num);
		printf("[2]lockcard on %d, result: %d\n", num, errcode);
		errcode = pci.unlockCard(num);
		printf("[1]unlockcard on %d, result: %d\n", num, errcode);
		printf("\n");

		//try unlocking a card that isn't locked
		printf("test: try unlocking a card that isn't locked\n");
		
		errcode = pci.unlockCard(num);
		printf("[1]unlockcard on %d, result: %d\n", num, errcode);
		printf("\n");
		
		//check that cards can be unlocked then relocked by another handle
		//try to lock a card in use
		printf("test: try to lock a card in use\n");
		errcode = pci.lockCard(num);
		printf("[1]lockcard on %d, result: %d\n", num, errcode);
		errcode = pci.unlockCard(num);
		printf("[1]unlockcard on %d, result: %d\n", num, errcode);
		errcode = pci2.lockCard(num);
		printf("[2]lockcard on %d, result: %d\n", num, errcode);
		printf("\n");    

		pci2.unlockCard(num);	
	
		//lock/unlock a card a large # of times (1000s), on all cards (each, scatter)
		printf("test: lock/unlock a card a large # of times (1000s)\n");
		for(int i=0; i < 1000; i++) {
			errcode = pci.lockCard(num);
			printf("%d/1000: [1]lockcard on %d, result: %d\t", i+1, num, errcode);
			errcode = pci.unlockCard(num);
			printf("         [1]unlockcard on %d, result: %d\n", num, errcode);
		}
		printf("\n");

		//try locking a nonexistant card (both by ID and position in array)
			//num=0, num > # of cards
			//(ID is hard to reliably test without knowing switches)	
		printf("test: try locking a nonexistant card (both by ID and position in array)\n");
		errcode = pci.lockCard(217);  // 217 is not possible with 6 switches... or at least it shouldn't
		printf("[1]lockcard on %d, result: %d\n", 217, errcode);
		printf("\n");
			
		//try closing file handle while holding lock (can another device open it?)
		printf("test: try closing file handle while holding lock (can another device open it?)\n");
		printf("[2] lockcard result: %d\n", pci2.lockCard(num) );
		printf("[2] freeHandles result: %d\n", pci2.freeHandles() );
		printf("\n");

		//can file descriptor variables be open/closed over and over?  (>255)
		printf("test: can file descriptor variables be open/closed over and over?  (>255)\n");
		pci2.freeHandles();
		for(int i=0; i < 300; i++) {
			printf(" open #%d: errorcode = %d\t", i+1, pci2.createHandles(device_name) );
			printf("close #%d: errorcode = %d\n", i+1, pci2.freeHandles() );
		}
		printf("\n");
	}

	//TODO: try lock/unlocking in different orders (1221, 1212)
   
    //try to read out from a card that isn't locked?  (is it possible?)
        //(check if locks are required before reading out)
}

void pci_dma_test() {
	
}

void pci_fork_test() {
	
}


void pci_speed_test() {
	printf("=== speed tests ===\n");
	stdPCI pci;
	pci.createHandles(device_name);


	for(unsigned int k=0; k < pci.getIDList().size(); k++) {

		pci.lockCard( pci.getIDList()[k] );
		pci.selectChannel(DEFAULT_CHAN);

		int MAX_BUFFERSIZE = pci.getMemoryLength( pci.getIDList()[k] );
		int SPEEDTEST_TARGETSIZE = 1048576*16;
		MAX_BUFFERSIZE = 8192*32;
	
		for(int i=256; i <= MAX_BUFFERSIZE; i = i << 1) {
			int bytes = 0, time = 0;
			char* tmpBuffer = new char[MAX_BUFFERSIZE];
			char* tmpBuffer2 = new char[MAX_BUFFERSIZE];

		        for(int j=0; j < MAX_BUFFERSIZE; j++)
				tmpBuffer[j] = rand() % 256;

			int bytes_read, bytes_sent, time_read, time_sent;
			bytes_read = bytes_sent = time_read = time_sent = 0;

			int match = 0;

			while (bytes_read < SPEEDTEST_TARGETSIZE) {
				
				start_timer();
				bytes_sent += pci.sendData(tmpBuffer, i);
				time_sent += stop_timer();

				start_timer();
				bytes_read += pci.readData(tmpBuffer2, i);
				time_read += stop_timer();

				for(int q=0; q < i; q++)
					if (tmpBuffer[q] != tmpBuffer2[q])
						match++;
	
				if (match == 1) {
					printf("(mismatch)");
					match = 2;
				}

			}
			time = stop_timer();

			delete[] tmpBuffer;

			printf("r/w %d bytes in %d byte blocks: %d us to send, %d us to read\n", bytes_read, i, time_sent, time_read);
		}

		// add tests for individual packet sizes
		const int packetsize = 130*4;
		const int readoutsize = 130*4*514;

		for(int i=packetsize; i <= MAX_BUFFERSIZE; i += packetsize) {
			int bytes = 0, time = 0;

			int bytes_read, bytes_sent, time_read, time_sent;
			bytes_read = bytes_sent = time_read = time_sent = 0;

			while (bytes_read < readoutsize) {
				char tmpBuffer[MAX_BUFFERSIZE];
				
				start_timer();
				bytes_sent += pci.sendData(tmpBuffer, i);
				time_sent += stop_timer();

				start_timer();
				bytes_read += pci.readData(tmpBuffer, i);
				time_read += stop_timer();
	
//				int delta = stop_timer();

//				printf("time to send a single %d byte packet: %d ms\n", i, delta);		

//				time += delta;
			}
			
			printf("time to r/w %d bytes (alternating) in %d byte blocks: %d us send, %d us read\n", bytes_read, i, time_sent, time_read);
		}


		pci.unlockCard( pci.getIDList()[k] );

	}
}

