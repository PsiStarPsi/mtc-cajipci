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

#define REG_NUM 8

#define DRV_NAME "/dev/cajipci0"

int reg[REG_NUM] = {0, 
0,
0,
0,
0,
0,
0,
0xF1E2C3B4};

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
		data &= ~(~3 << SPI_SEL);
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
	close(fd);
}
