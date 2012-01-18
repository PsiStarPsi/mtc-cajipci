#ifndef ALTIX_USERLAND_HEADER
#define ALTIX_USERLAND_HEADER

#include <linux/types.h>

///IOCTL deffinitions
#define ALTIX_IOCTL_NUM 0x881
#define ALTIX_IOCTL_INFO 0x882
#define ALTIX_IOCTL_LOCK 0x883
#define ALTIX_IOCTL_RELEASE 0x884
#define ALTIX_IOCTL_STAT 0x885
#define ALTIX_IOCTL_DMA 0x887
#define ALTIX_IOCTL_CHAN 0x888

/**
 * Card information data structure. Used in the IOCTL call ALTIX_IOCTL_INFO.
 */
typedef struct
{
	uint id; /**< Card ID. */
	int pid; /**< PID of a process using this card. 0 if free. */
	int memlen; /**< Memory window for this card. */
	int channel;
} altix_pci_card_info;


//Performance monitoring

/**
 * Driver performance structure. Used in  IOCTL call ALTIX_IOCTL_INFO.
 */
typedef struct 
{
	int id;	/**< Card ID. */
	unsigned int num_reads;	/**< Number of reads which occurred on this card */
	unsigned int num_writes; /**< Number of writes which occurred on this card */
	uint64_t  bytes_written;	/**< Number of bytes writen */
	uint64_t bytes_read;		/*** < Number of bytes read */
} altix_pci_card_stat;

#endif
