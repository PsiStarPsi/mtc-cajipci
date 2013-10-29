#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#define CAJIPCIDEV "/dev/cajipci0"
#define TRGOFFSET 16

int usage(char* name)
{
	printf("%s is used to configure the CAJIPCI trigger", name);
	printf("%s register_mask", name);
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		usage(argv[0]);
		return -1;
	}

	uint16_t mask = strtol(argv[1], NULL, 16);
	int fd = open(CAJIPCIDEV, O_RDWR);
	if(fd < 0)
	{
		printf("Could not open the cajipci device %s.\nAre you sure the driver is loaded?\n", CAJIPCIDEV);
		return -1;
	}
	lseek(fd, TRGOFFSET , SEEK_SET);
	uint32_t reg;
	read(fd, &reg, sizeof(uint32_t));
	reg &= 0xFFFF; //clear low bits;
	reg |= mask;

	lseek(fd, TRGOFFSET , SEEK_SET);
	write(fd, &reg, sizeof(uint32_t));
	return 0;
}
