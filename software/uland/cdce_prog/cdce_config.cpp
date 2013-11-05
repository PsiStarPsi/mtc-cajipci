#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp>

using namespace std;

#define SEEK_SEND 0l
#define SEEK_RCV 0x4l
#define SEEK_CTL 0x8l

#define SPI_START 0
#define SPI_DONE  1
#define SPI_SEL   2
#define SPI_SYNC  4

#define REG_NUM 9

#define DRV_NAME "/dev/cajipci0"

bool readConfigFile( std::string fname, int *regs )
{
	std::string::size_type idx = fname.rfind('.');
	if(idx == std::string::npos)
	{
		cout << "Config file must have .INI extension" << endl;
		return false;
	}
    std::string extension = fname.substr(idx+1);
    if(extension != "INI")
	{
		if(extension != "ini")
		{
			cout << "Config file must have .INI extension" << endl;
			return false;		
		}
	}
	FILE* f = fopen(fname.c_str(), "r");
	if( f == NULL)
	{
		cout << "Config file must have .INI extension" << endl;
		return false;
	}
	char buf[100];
	string header("\xef\xbb\xbfREGISTERS");
	fgets(buf, 99, f);
	std::string fileHeader(buf);
	boost::algorithm::trim(fileHeader);
	boost::algorithm::trim_right(fileHeader);
	if(header != fileHeader)
	{
		cout << "Config file bad header " << fileHeader.length() << " instead of " << header.length() << endl;
		return -1;
	}
	for(int i = 0; i< 9; i++)
	{
		int d;
		fscanf(f, "%d\t%x", &d, &regs[i]);
	}
	return true;
}

void usage(char* name)
{
	cout << name << " [config_file] [jitter_cleaner_number 0-2]" << endl;
}

static const int readReg[REG_NUM] = { 
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


bool writeRegs(int* regs, int chan)
{
	int fd = open(DRV_NAME, O_RDWR);
	if(fd < 0)
	{
		cerr << "Could not open the cajipci driver " << DRV_NAME << endl;
		exit(-1);
	}
	
	chan &= 3;
	
	for(int i = 0; i < REG_NUM; i++)
	{
		
		int data = regs[i];
		lseek(fd, SEEK_SEND, SEEK_SET);
		write(fd, &data, sizeof(int));
		usleep(100);
		data = chan << SPI_SEL;
		lseek(fd, SEEK_CTL, SEEK_SET);
        write(fd, &data, sizeof(int));
        usleep(100);   
		data &= ~(1 << SPI_START);
		lseek(fd, SEEK_CTL, SEEK_SET);
        write(fd, &data, sizeof(int));
		usleep(100);
		data |= 1 << SPI_START;
		lseek(fd, SEEK_CTL, SEEK_SET);
		write(fd, &data, sizeof(int)); 
		usleep(100);
	}
	::close(fd);
}

bool readRegs(int chan)
{
	int fd = open(DRV_NAME, O_RDWR);
	if(fd < 0)
	{
		cerr << "Could not open the cajipci driver " << DRV_NAME << endl;
		exit(-1);
	}
	
	chan &= 3;
	
	for(int i = 0; i < REG_NUM; i++)
	{
		
		int data = readReg[i];
		lseek(fd, SEEK_SEND, SEEK_SET);
		write(fd, &data, sizeof(int));
				
		data = chan << SPI_SEL;
		lseek(fd, SEEK_CTL, SEEK_SET);
        write(fd, &data, sizeof(int));
        
        
		data &= ~(1 << SPI_START);
		lseek(fd, SEEK_CTL, SEEK_SET);
        write(fd, &data, sizeof(int));
	
		data |= 1 << SPI_START;
		lseek(fd, SEEK_CTL, SEEK_SET);
		write(fd, &data, sizeof(int)); 
		usleep(100);

		lseek(fd, SEEK_RCV, SEEK_SET);
		read(fd, &data, sizeof(int));
		printf("%d: %x\n", i, data); 
	}
	::close(fd);	
}


int main(int argc, char** argv)
{
	if(argc != 3)
	{
		usage(argv[0]);
		return -1;
	}
	char* end;
	uint chan =  strtol (argv[2], &end, 0);
	if(*end != '\0')
	{
		usage(argv[0]);
		return -1;		
	}
	if(chan >3)
	{
		usage(argv[0]);
		return -1;				
	}
	int writeReg[REG_NUM] = {0};
	if(!readConfigFile(argv[1], writeReg))
	{
		usage(argv[0]);
		return -1;		
	}
	writeRegs(writeReg, chan);
	readRegs(chan);
	
	int fd = open(DRV_NAME, O_RDWR);
	if(fd < 0)
	{
		cerr << "Could not open the cajipci driver " << DRV_NAME << endl;
		return -1;
	}
	
	
	int data;
	::lseek(fd, SEEK_CTL, SEEK_SET);
	::read(fd, &data, sizeof(int));
	data |= 1 << SPI_SYNC;
	printf("Syncing...\n");
	lseek(fd, SEEK_CTL, SEEK_SET);
	write(fd, &data, sizeof(int));
	usleep(100);
	data = 0;
	lseek(fd, SEEK_CTL, SEEK_SET);
	write(fd, &data, sizeof(int));
	close(fd);
	return 0;
}
