#include "libbeamdata.h"
#include "stdlib.h"
#define LIB_BEAM_VERSION 1

#define DAC_RESOLUTION 0.000610500611

#define CHANNELS_PER_FIBER 512
#define SAMPLES_PER_CHANNEL 256
#define CHUNK_SIZE 140
#define CHUNK_NUM 514

#define WAVE_PACKET_TYPE 0x00C0FFEE
#define HOUSE_PACKET_TYPE 0x000AB0DE
#define TRIG_DAC_PACKET_TYPE 0x1EBE1DEF
#define TRIG_STR_PACKET_TYPE 0xCE11B10C

//Global packet
#define TYPE_OFFSET 0x2
#define SIZE_OFFSET 0x1
//Wave packet
#define EVENT_NUMBER_OFFSET 0x5
#define CHANNEL_ID_OFFSET 0x6
#define WAVE_ADDRESS OFFSET 0xB
#define WAVEFORM_OFFSET 0xC

beam_pixel_data * beam_process_pixel_data(int* pixel_data)
{
	int i;
	int temp;
	beam_pixel_data * pixel = (beam_pixel_data*)malloc(sizeof(beam_pixel_data));
	pixel->data =(float*)malloc(sizeof(float)*SAMPLES_PER_CHANNEL);
	pixel->event_number = pixel_data[EVENT_NUMBER_OFFSET];
	//id
	temp = pixel_data[CHANNEL_ID_OFFSET];
	pixel->chan_id = temp & 0x0000000F;
	pixel->asic_id =  (temp & 0x000000F0) >> 8;
	for(i = 0;  i < SAMPLES_PER_CHANNEL/2; i++)
	{
		temp = pixel_data[WAVEFORM_OFFSET + i];
		pixel->data[i] = (temp & 0x00000FFF)*DAC_RESOLUTION;
		pixel->data[i] = ((temp >> 16) & 0x00000FFF)*DAC_RESOLUTION;
	}
	return pixel;
}

beam_scrod_data * beam_initialize_event(int* event_packet)
{
	int i;
	int packet_type;
	beam_scrod_data *event;
	event = (beam_scrod_data *)malloc(sizeof(beam_scrod_data));
	event->pixels = (beam_pixel_data*)malloc(sizeof(beam_pixel_data)*CHANNELS_PER_FIBER);
	for(i = 0; i < CHUNK_NUM; i++)
	{
		packet_type = event_packet[i*CHUNK_SIZE + TYPE_OFFSET];
		switch(packet_type)
		{
			case WAVE_PACKET_TYPE: 
				beam_process_pixel_data(event_packet[CHUNK_SIZE*i]);
				break;
			case HOUSE_PACKET_TYPE:
				break;
			case TRIG_DAC_PACKET_TYPE:
				break;
			case TRIG_STR_PACKET_TYPE:
				break;
		}
	}
	return NULL;
}

int beam_get_verstion()
{
	return LIB_BEAM_VERSION;
}
