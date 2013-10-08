#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define SEEK_SEND 0l
#define SEEK_RCV 0x4l
#define SEEK_CTL 0x8l

#define SPI_START 0
#define SPI_DONE  1
#define SPI_SEL   2
#define SPI_SYNC  4

#define REG_NUM 18

#define DRV_NAME "/dev/cajipci0"

int reg[REG_NUM] = { 
	0xEB340320,
	0xEB340301,
	0xEB340302,
	0xEB340303,
	0x68860314,
	0x10000A65,
	0x04CE0C76,
	0xBD887AA7,
	0x80001808,
	0x0E,
	0x1E,
	0x2E,
	0x3E,
	0x4E,
	0x5E,
	0x6E,
	0x7E,
	0x8E
};

using namespace std;

int main()
{
	int fd = open(DRV_NAME, O_RDWR);
	if(fd < 0)
	{
		cerr << "Could not open the dumb pci driver " << DRV_NAME << endl;
		exit(-1);
	}
	for(int i = 0; i < REG_NUM; i++)
	{
		int  data = 0;
                lseek(fd, SEEK_CTL, SEEK_SET);
                write(fd, &data, sizeof(int));
		data = reg[i];
		lseek(fd, SEEK_SEND, SEEK_SET);
		write(fd, &data, sizeof(int));

	        lseek(fd, SEEK_CTL, SEEK_SET);
		read(fd, &data, sizeof(int));
		data &= (0 << SPI_SEL);

                lseek(fd, SEEK_CTL, SEEK_SET);
		data &= ~(1 << SPI_START);
                write(fd, &data, sizeof(int));
	
		lseek(fd, SEEK_CTL, SEEK_SET);
		data |= 1 << SPI_START;
		write(fd, &data, sizeof(int)); 
		
		usleep(100);
		lseek(fd, SEEK_RCV, SEEK_SET);
		read(fd, &data, sizeof(int));
		printf("%d: We wrote %x and read %x\n", i, reg[i], data);
	}
	int data = 1 << SPI_SYNC;
	printf("Syncing...\n");
        lseek(fd, SEEK_CTL, SEEK_SET);
	write(fd, &data, sizeof(int));
	usleep(100);
	data = 0;
	lseek(fd, SEEK_CTL, SEEK_SET);
	write(fd, &data, sizeof(int));
	close(fd);
}
