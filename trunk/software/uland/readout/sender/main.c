#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libaltix.h"

#define DEVICE_PATH "/dev/altixpci0"

void print_usage()
{
	printf("sender -c <channel> -b <board_id> -f <file> <-w> <-r> <-n> <-p>\n");
	printf("-b:		board id.\n");
	printf("-c:		channel.\n");
	printf("-f:		file to store or send.\n");
	printf("-w:		write data, default is read.\n");
	printf("-r:		reset channel\n");
	printf("-n:		number of bytes to read or write. Default is event size for reading and packet size for writing\n");
	printf("-p:		Read the pulse register for a card.\n");
}

void print_cards()
{
	print_usage();
	int fd;
	int error;
	altix_pci_card_info* infos;
	int i;
	int num_cards = altix_initialize(DEVICE_PATH);
	if(num_cards <= 0)
	{
		printf("No cards found\n");
		abort();
	}
	fd = open(DEVICE_PATH, O_RDWR);
	if(fd <0)
	{
		printf("Cant open device handle\n");
		abort();
	}
	infos = altix_get_cards(fd, &error);
	const altix_pci_card_stat* stats = altix_get_stats(fd, &error);
	printf("Found %d card(s)\n", num_cards);
	for(i = 0; i< num_cards; i++)
	{
		printf("Card %d\n", i+1);
		printf("	ID:		%d\n", infos[i].id);
		if(infos[i].pid == 0)
			printf("	Not Locked\n");
		else
			printf("	Locked by:	%d\n", infos[i].pid);
		printf("	Bytes written:	%llu\n", stats[i].bytes_written);
		printf("	Bytes read:	%llu\n", stats[i].bytes_read);
	}
	printf("Done, See ya\n");
	exit(0);
}

int main(int argc, char** argv)
{
	int chan = -1;
	int board = -1;
	char* file = NULL;
	int read_run = 1;
	char c;
	int fd;
	int data;
	int * buff;
	int reset = 0;
	int num_bytes = 0;
	int read_pulse = 0;
	if(argc == 1)
	{
		print_cards();
	}
	while ((c = getopt (argc, argv, "rpwc:b:f:n:")) != -1)
	switch (c)
	{
		case 'c':
			chan = atoi(optarg);
			break;
		case 'b':
			board = atoi(optarg);
			break;
		case 'f':
			file = optarg;
			break;
		case 'r':
			reset = 1;
			break;
		case 'w':
			read_run = 0;
			break;
		case 'n':
			num_bytes = atoi(optarg);
			break;
		case 'p':
			read_pulse = 1;
			break;
		case '?':
			fprintf (stderr,"Unknown option character `%x'.\n", optopt);
			print_usage();
			return 1;
		default:
			fprintf (stderr,"Unknown option character `%x'.\n", optopt);
			print_usage();
			return 1;
	}
	if(chan < 0 || board <0 || (file  == NULL && reset == 0 && read_pulse == 0))
	{
		print_usage();
		return 0;
	}
	if(altix_initialize(DEVICE_PATH) <= 0)
	{
		printf("No cards found\n");
		abort();
	}
	fd = open(DEVICE_PATH, O_RDWR);
	if(fd <0)
	{
		printf("Cant open device handle\n");
		abort();
	}
	if(altix_lock_card(fd, board) != ALTIX_OK)
	{
		printf("Could not lock card\n");
		abort();
	}
	if(read_pulse)
	{
		int status;
		if(altix_read_pulse(fd, &status) != ALTIX_OK)
		{
			printf("Could not read status register\n");
			abort();
		}
		else
		{
			printf("Status register for board %d is: 0x%x", board, status);
		}
	}
	altix_toggle_dma(fd);
	altix_switch_channel(fd, chan);
	if(reset)
	{
		printf("Resetting board %d channel %d\n", board, chan);
		altix_enable_chan(fd, 0);
		altix_enable_chan(fd, 1);
	}
	if(file != NULL)
	{
		if(read_run)
		{
			if(num_bytes == 0)
				num_bytes = altix_event_size();
			printf("Reading %b bytes from board %d, channel %d, to file %s\n", num_bytes, board, chan, file);
			buff = (int*)malloc(num_bytes);
			data = open(file, O_WRONLY | O_CREAT);
			if(data <0)
			{
				printf("Can`t open output file %s", file);
				abort();
			}
			read(fd, buff, num_bytes);
			write(data, buff, num_bytes);
			free(buff);
		}
		else
		{
			data = open(file, O_RDONLY);
			if(data <0)
			{
				printf("Can`t open input file %s\n", file);
				abort();
			}

			if(num_bytes == 0)
			{
				struct stat size;
				fstat(data, &size);
				num_bytes = size.st_size;
			}
			printf("Writing %d bytes to board %d, channel %d, form file %s\n", num_bytes, board, chan, file);
			buff = (int*)malloc(num_bytes);
			read(data, buff, num_bytes);
			write(fd, buff, num_bytes);
			free(buff);
		}
	}
	printf("Done, See ya\n");
	exit(0);
}
