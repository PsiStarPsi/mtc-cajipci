#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#define CAJIPCIDEV "/dev/cajipci0"
#define TRGSTATOFFSET 0x14

int main(int argc, char** argv)
{
	int fd = open(CAJIPCIDEV, O_RDWR);
	if(fd < 0)
	{
		printf("Could not open the cajipci device %s.\nAre you sure the driver is loaded?\n", CAJIPCIDEV);
		return -1;
	}
	lseek(fd, TRGSTATOFFSET , SEEK_SET);
	uint32_t reg;
	read(fd, &reg, sizeof(uint32_t));
	printf("%u\n", reg);
	return 0;
}
