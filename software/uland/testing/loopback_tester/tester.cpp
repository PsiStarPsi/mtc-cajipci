#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "stdPCI.h"

#define MAX_CONNECTIONS 100
#define DEVICE_NAME  "/dev/altixpci0"

//#define VERBOSE_LOGGING

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

int main(int argc, char* argv[]) {
#ifdef VERBOSE_LOGGING
	FILE* fout = fopen("error.txt", "w");
#endif

	// parse connections
	int from_id[MAX_CONNECTIONS], to_id[MAX_CONNECTIONS], 
		from_ch[MAX_CONNECTIONS], to_ch[MAX_CONNECTIONS];

	for(int i=1; i < argc; i++) {
		sscanf(argv[i], "%d:%d-%d:%d", &from_id[i-1], &from_ch[i-1], &to_id[i-1], &to_ch[i-1]);
		printf("read link from id%d ch%d to id%d ch%d\n", from_id[i-1], from_ch[i-1], to_id[i-1], to_ch[i-1]);
	}	
	int num_links = argc-1;
	
	if (argc == 1) {
		printf("specify links, format:   id:ch-id:ch \n");
		return -1;
	}

	unsigned char in_use[257];
	stdPCI cards[257];  // id maxes out at 216 but leaving some buffer space just in case

	for(int i=0; i < 257; i++)
		in_use[i] = 0;

	for(int i=0; i < num_links; i++) {
		in_use[from_id[i] ]++;
		in_use[ to_id[i] ]++;
	}

	for(int i=0; i < 257; i++) {
		if (in_use[i] > 0) {
			cards[i].createHandles(DEVICE_NAME);
			cards[i].lockCard(i);
		}
	}
	
	int bytes=0, target=1048*1048576;
	int blocksize = 65536;

	for(int i=0; i < num_links; i++) {
		bytes = 0;
		printf("testing link %d\n", i);
		long long read_time = 0, write_time = 0;
		int mismatch = 0;

		while (bytes < target) {
			char buffer[65536], buffer2[65536];

			int from, to;
			from = from_id[i];
			to = to_id[i];

			// send data
			start_timer();
			cards[ from_id[i] ].selectChannel(from_ch[i]);
			cards[ from_id[i] ].sendData(buffer, blocksize);
			write_time += stop_timer();

			// read data back
			start_timer();
			cards[ to_id[i] ].selectChannel(to_ch[i]);
			int count = cards[ to_id[i] ].readData(buffer2, blocksize);
			read_time += stop_timer();

			// verify data
			bool flag = false;

			for(int j=0; j < blocksize; j++) {
				if (buffer[j] != buffer2[j]) {
					mismatch++;
					flag = true;
				}
			}

#ifdef VERBOSE_LOGGING
			if (flag) {
				for(int j=0; j < blocksize; j++) {
					if (buffer[j] == buffer2[j]) {
						fprintf(fout, "[%02x] ", (unsigned char)buffer[j] );
					}
					else {
						fprintf(fout, "<%02x %02x> ", (unsigned char)buffer[j],
									(unsigned char)buffer2[j] );
					}
					fflush(fout);
				}
				fprintf(fout, "\n");
				fflush(fout);
			}
#endif

			// update screen
			bytes += count;
			printf("%d/%d bytes sent/received\r", bytes, target);
		}
		printf("\n");
		printf("write time: %d us, read time: %d us\n\n", write_time, read_time);
	}

}
