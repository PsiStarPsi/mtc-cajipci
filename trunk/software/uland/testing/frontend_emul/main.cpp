#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
#include "libaltix.h"
}
#define DEVICE_PATH "/dev/altixpci0"
#define PACKET_PATH "00C0FFEE.packet"
int main(int argc, char** argv)
{
	int card;
	int res, fd, data;
	char * wrdata;
	char * rddata;
	int error;
	int chan_activity = 0xFFFFFFFF;
	if(argc != 2)
	{
		printf("%s card_id\n", argv[0]);
		exit(0);
	}
	card = atoi(argv[1]);
	if(altix_initialize((char*)DEVICE_PATH) < 0)
	{
		printf("Can't initialize the library!\n");
		exit(0);
	}
	fd = ::open((char*)DEVICE_PATH, O_RDWR);
	altix_get_cards(fd, &res);
	error = altix_lock_card(fd, card);
	if(error != ALTIX_OK)
	{
			fprintf(stderr, "Cant lock card %d\n", card);
			exit(-1);
	}
	fprintf(stderr, "Locked card %d\n", card);
	altix_toggle_dma(fd);
	fprintf(stderr, "Resetting all channels\n");
	for(int i = 0; i< 4; i++)
	{
		altix_switch_channel(fd, i);
		altix_enable_chan(fd, 0);
		altix_enable_chan(fd, 1);
	}
	wrdata = (char*) malloc(altix_event_size());
	rddata = (char*) malloc(altix_packet_size());
	data = ::open((char*)PACKET_PATH, O_RDONLY);
	::read(data, rddata, altix_packet_size());
	::close(data);
	for(int i = 0; i< altix_event_size()/altix_packet_size(); i++)
	{
		memcpy(wrdata + i* altix_packet_size(), rddata, altix_packet_size());
	}
	fprintf(stderr, "Starting loop\n");
	while(1)
	{
		for(int chan = 0; chan < 4; chan++)
		{
			altix_switch_channel(fd, chan);
			if(chan_activity & (1 << chan))
			{
				printf("Wrote %d\n", ::write(fd, wrdata, altix_event_size()));
			}
		}
		for(int chan = 0; chan< 4; chan++)
		{
			altix_switch_channel(fd, chan);
			if(altix_poll_read(fd))
			{
				chan_activity |= 1 << chan;
				::read(fd, rddata, altix_packet_size());
			}
			else
			{
				chan_activity &= ~(1 << chan);
			}
		}
	}
}
