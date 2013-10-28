#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h> 

#define CAJIPCIDEV "/dev/cajipci"
#define JTAGOFFSET 12

int usage(char* name)
{
	printf("%s jtag_channel\n", name);
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		usage(argv[0]);
		return -1;
	}
	int chan = atoi(argv[1]);
	int fd = open(CAJIPCIDEV, O_WRONLY);
	if(fd < 0)
	{
		printf("Could not open the cajipci device %s.\nAre you sure the driver is loaded?\n", CAJIPCIDEV);
		return 0;
	}
	lseek(fd, JTAGOFFSET, SEEK_SET);
	return 0;
}
