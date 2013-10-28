#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#define CAJIPCIDEV "/dev/cajipci"
#define TRGOFFSET 16
#define SOFTTRGPOS 16

int main(int argc, char** argv)
{
	int fd = open(CAJIPCIDEV, O_WRONLY);
	if(fd < 0)
	{
		printf("Could not open the cajipci device %s.\nAre you sure the driver is loaded?\n", CAJIPCIDEV);
		return -1;
	}
	lseek(fd, SOFTTRGPOS , SEEK_SET);
	uint32_t reg;
	read(fd, &reg, sizeof(uint32_t));
	reg &= ~(1 << SOFTTRGPOS);	//clear the bit
	lseek(fd, SOFTTRGPOS , SEEK_SET);
	write(fd, &reg, sizeof(uint32_t));
	reg |= 1 << SOFTTRGPOS;		//set the bit
	lseek(fd, SOFTTRGPOS , SEEK_SET);
	write(fd, &reg, sizeof(uint32_t));
	return 0;
}
