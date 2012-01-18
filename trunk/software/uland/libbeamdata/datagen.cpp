#include "datagen.h"
#include <stdio.h>

#define FREEZE_DATE 0x20110620

unsigned short sine[91] = {0, 8, 17, 26, 34, 43, 52, 60, 69, 78, 86, 95, 
	103, 112, 120, 129, 137, 146, 154, 162, 171, 179, 187, 195, 203, 211, 
	219, 226, 234, 242, 250, 257, 264, 272, 279, 286, 293, 300, 307, 314, 
	321, 328, 334, 340, 347, 353, 359, 365, 371, 377, 383, 388, 394, 399, 
	404, 409, 414, 419, 424, 428, 433, 437, 441, 445, 449, 453, 456, 460, 
	463, 466, 469, 472, 475, 478, 480, 482, 485, 487, 489, 490, 492, 493, 
	495, 496, 497, 498, 498, 499, 499, 499, 500 };

unsigned int SamplePtr = 0;
unsigned short GetWaveformSample() {
	int pos = (SamplePtr++) % 360;
	if (pos <= 90)
		return 1000 + sine[pos];
	else if (pos <= 180)
		return 1000 + sine[180 - pos];
	else if (pos <= 270)
		return 1000 - sine[pos - 180];
	else if (pos <= 360)
		return 1000 - sine[360 - pos];
}

//unsigned int buffer[140];
unsigned int *buffer;

int event_num = 0;
int chan = -1;

int finalizePacket(RawPacket &packet) {
	buffer[4] = 0;
	unsigned int checksum = 0;	

	for(int i=0; i < 140; i++) {
		checksum += buffer[i];
	}

	checksum = (0xffffffff - checksum) + 1;
	buffer[4] = checksum;
}

int generateBasePacket(RawPacket &packet) {
	buffer = (unsigned int*)&packet;

	buffer[0] = 0x00be11e2;
	buffer[1] = 0x8c;
	buffer[2] = 0;
	buffer[3] = FREEZE_DATE ;
	buffer[4] = 0x0;

	buffer[139] = 0x62504944;
}

int generateWaveformPacket(RawPacket &packet) {
	buffer = (unsigned int*)&packet;
	buffer[2] = 0x00c0ffee;

	chan++;
	if (chan >= 512) {
		chan = 0;
		event_num++;
	}

	int scrod, asic, ch;
	ch = chan % 8;
	scrod = chan / 128;
	asic = (chan / 8) % 16;

	buffer[6] = ch | (asic << 8) | (scrod << 16);

	for(int i=0; i < 128; i++) {
		buffer[11+i] = GetWaveformSample();  // waveform data
		buffer[11+i] += GetWaveformSample() << 16;
	}
}

int generateHousekeepingPacket(RawPacket &packet) {
	buffer = (unsigned int*)&packet;
	buffer[2] = 0x000ab0de;
}

int generateTriggerStreamPacket(RawPacket &packet) {
	buffer = (unsigned int*)&packet;
	buffer[2] = 0xce11b10c;
}

int main() {
	RawPacket foo;

	unsigned char* ptr = (unsigned char*)&foo;

	for(int i=0; i < sizeof(RawPacket); i++)
		ptr[i] = 0;

	for(int k=0; k < 512; k++) {
		printf("==========PACKET %d============\n", k+1);

		generateBasePacket(foo);
		generateWaveformPacket(foo);
		finalizePacket(foo);

		for(int i=0; i < sizeof(RawPacket); i++) {
			printf("%02x ", ptr[i]);
			if (( (i+1) % 4) == 0) printf(" ");
			if (( (i+1) % 16) == 0) printf("\n");
		}	

		printf("struct test:\n");
		printf("Header: %08x\n", foo.header);
		printf("Size: %d\n", foo.size);
		printf("Packet type: %d\n", foo.type);
		printf("Protocol freeze date: %08x\n", foo.date);
		printf("Checksum: %08x\n", foo.checksum);
		
		for(int i=0; i < 256; i++)
			printf("Waveform data %d: %d\n", i, foo.data.wave.waveform_ADC[i]);
		
		printf("Footer: %08x\n", foo.footer);
		
		
	}
	return 0;
}
