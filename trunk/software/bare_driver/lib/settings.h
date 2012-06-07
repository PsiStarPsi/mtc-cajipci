#ifndef _BLAB_GLOBAL_CONFIG
#define _BLAB_GLOBAL_CONFIG
#define BUFFSIZE 526
#define BUFFNUM 8
#include <linux/types.h>
#include <linux/semaphore.h>

struct blab_buffered_data
{
	void* data[BUFFNUM];
	int* data_stamp;
	int w_pos;
	int r_pos;
	int stamp;
	struct semaphore * rw_sem;
};

#endif /* settings */
