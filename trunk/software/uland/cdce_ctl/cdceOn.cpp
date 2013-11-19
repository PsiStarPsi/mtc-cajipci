#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp>

using namespace std;

#define SEEK_CTL 0x8l

#define SPI_SYNC  4


#define DRV_NAME "/dev/cajipci0"


int main(int argc, char** argv)
{
	int fd = open(DRV_NAME, O_RDWR);
	if(fd < 0)
	{
		cerr << "Could not open the cajipci driver " << DRV_NAME << endl;
		return -1;
	}
	int data =0;
	::lseek(fd, SEEK_CTL, SEEK_SET);
	printf("Enabling Clock\n");
	write(fd, &data, sizeof(int));
	close(fd);
	usleep(100);
	return 0;
}
