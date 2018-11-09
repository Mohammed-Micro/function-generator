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
	unsigned long freq;
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

	//Define type pointer to function that returns char* and takes (unsgined long,unsigned long,char **)
	typedef char *(*generate_ptr)(unsigned long,unsigned long,char **);
	//Array of pointers to functions of type generate_ptr
	generate_ptr generates[] = {&generate_sine,&generate_square,&generate_sawtooth,&generate_triangular};
	//Define pointers that can be used to call the appropriate generate_<wave> function for each channel
	generate_ptr ch1_generate_wave_ptr = generates[ch1_wave->shape];
	generate_ptr ch2_generate_wave_ptr = generates[ch2_wave->shape];

	while(1){
		rc_ptr = (*ch1_generate_wave_ptr)(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
		check(rc_ptr,"rc_ptr is NULL");
		rc_ptr = (*ch2_generate_wave_ptr)(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
		check(rc_ptr,"rc_ptr is NULL");

		buffer = play(ch1_samples,ch2_samples);
		check(buffer,"buffer is NULL");
	}
	return 0;
error:
	if(ch1_samples)
		free(ch1_samples);
	if(ch2_samples)
		free(ch2_samples);
	if(buffer){
		free(buffer);

	return -1;
}

int main(int argc,char **argv){

	return 1;
}


