#ifndef LIB_BEAM_DATA_HEADER
#define LIB_BEAM_DATA_HEADER

#include <stdio.h>

typedef struct
{
	int event_number;
	int chan_id;
	int asic_id;
	int wave_address;
	float *data;
} beam_pixel_data;

typedef struct
{
	int scrod_id;
	int event_number;
	int orbit_number;
	int version;
	beam_pixel_data * pixels;
} beam_scrod_data;

beam_scrod_data * beam_initialize_event(int* event);
beam_pixel_data * beam_process_pixel_data(int* pixel_data);
int beam_get_verstion();

#endif
