#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include "dbg.h"
#include "playback.h"
#include "wave.h"


typedef enum _shape{
	SINE = 0,SQUARE,SAWTOOTH,TRIANGULAR
}Shape;

typedef struct _wave{
	Shape shape;
	unsigned int freq;
	unsigned long amp;
}Wave;

static int err;

const char* wave_names[] = {"Sine","Square","Sawtooth","Triangular"};

int output_waves(Wave *ch1_wave,Wave *ch2_wave){

	/*These two pointers point to the memory allocated by generate_<wave> functions
	 * These allocated chunks of memory stores the data to be produced using function play
	 */
	char *ch1_samples = NULL, *ch2_samples = NULL;
	/*Holds the returned pointer from function play*/
	char *ring_buffer = NULL;
	/*Checks if calls to functions generate_<wave> was successfull or not*/
	char *rc_ptr = NULL;

	switch(ch1_wave->shape){
		case SINE:
			rc_ptr = generate_sine(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
			break;
		case SQUARE:
			rc_ptr = generate_square(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
			break;
		case SAWTOOTH:
			rc_ptr = generate_saw(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
			break;
		case TRIANGULAR:
			rc_ptr = generate_triangular(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
			break;
		default:
			sentinel("This value(%d) doesn't represent a waveform",ch1_wave->shape);
	}
	check(rc_ptr,"rc_ptr in NULL");

	//Do same thing for channel 2
	switch(ch2_wave->shape){
		case SINE:
			rc_ptr = generate_sine(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
			break;
		case SQUARE:
			rc_ptr = generate_square(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
			break;
		case SAWTOOTH:
			rc_ptr = generate_saw(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
			break;
		case TRIANGULAR:
			rc_ptr = generate_triangular(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
			break;
		default:
			sentinel("This value(%d) doesn't represent a waveform",ch2_wave->shape);
	}
	check(rc_ptr,"rc_ptr is NULL");
	return 0;
error:
	return -1;
}

int main(int argc,char **argv){

	return 1;
}


