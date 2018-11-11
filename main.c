#define NDEBUG

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include <errno.h>
#include "dbg.h"
#include "playback.h"
#include "wave.h"

#define WAVES 4

typedef enum _shape{
	SINE = 0,SQUARE,SAW,TRIANG
}Shape;

//Structure holding the parameter of a waveform
typedef struct _wave{
	Shape shape;
	unsigned long freq;
	unsigned long amp;
}Wave;

static int err;

//Map integers in Shape enum to strings 
const char* wave_names[] = {"sine","square","saw","triang"};

/* Generate and play a waveform for each channle */
int output_waves(Wave *ch1_wave,Wave *ch2_wave){

	/*These two pointers point to the memory allocated by generate_<wave> functions
	 * These allocated chunks of memory stores the data to be produced using function play
	 */
	char *ch1_samples = NULL, *ch2_samples = NULL;
	/*Holds the returned pointer from function play*/
	char *buffer = NULL;
	/*Checks if calls to functions generate_<wave> was successfull or not*/
	char *rc_ptr = NULL;

	//Define type pointer to function that returns char* and takes (unsgined long,unsigned long,char **)
	typedef char *(*generate_ptr)(unsigned long,unsigned long,char **);
	//Array of pointers to functions of type generate_ptr
	generate_ptr generates[] = {&generate_sine,&generate_square,&generate_saw,&generate_triang};
	//Define pointers that can be used to call the appropriate generate_<wave> function for each channel
	generate_ptr ch1_generate_wave_ptr = generates[ch1_wave->shape];
	generate_ptr ch2_generate_wave_ptr = generates[ch2_wave->shape];

	while(1){
		//Call generate_<wave> functions using pointers to them
		rc_ptr = (*ch1_generate_wave_ptr)(ch1_wave->freq,ch1_wave->amp,&ch1_samples);
		check(rc_ptr,"generate_%s",wave_names[ch1_wave->shape]);
		rc_ptr = (*ch2_generate_wave_ptr)(ch2_wave->freq,ch2_wave->amp,&ch2_samples);
		check(rc_ptr,"generate_%s",wave_names[ch2_wave->shape]);

		buffer = play(ch1_samples,ch2_samples);
		check(buffer,"play");
	}

	if(ch1_samples)
		free(ch1_samples);
	if(ch2_samples)
		free(ch2_samples);
	if(buffer)
		free(buffer);

	return 0;
error:
	if(ch1_samples)
		free(ch1_samples);
	if(ch2_samples)
		free(ch2_samples);
	if(buffer)
		free(buffer);

	return -1;
}

/*Map a string holding the name of the desired waveform to an integer number
 * ,or '-1' if the name is not a valid waveform
 */
int wave_atoi(const char *wave_name){

	size_t i;
	for(i = 0;i < WAVES;i++){
		if(strcasecmp(wave_name,wave_names[i]) == 0){
			return i;//Return the index of the matched waveform name in wave_names array
		}
	}

	return -1;//No matches can be found
}

/*Explains itself,fools*/
void print_wave_params(Wave *wave){

	printf("Waveform: %s\n",wave_names[wave->shape]);
	printf("Frequency: %lu\n",wave->freq);
	printf("Amplitude: %lu\n",wave->amp);
}

int main(int argc,char **argv){

	int i;
	long rc;//shorthand for retern_check

	/*Used to indicate if a string contains no digits at the beginning */
	char *strtol_ptr;
	//Setting up a default waveform to play incase nothing is specified for a particular channel
	Wave ch1 = {.shape = SINE,.freq = 1000,.amp = 0};
	Wave ch2 = ch1;

	/*This loop basically looks for '-1' and '-2' */
	for(i = 1;i < argc;i++){
		char *arg = argv[i];
		debug("i: %u",i);

		if(strcmp(arg,"-1") == 0){
			/* There must be three or more arguments following '-1' option
			 * Note that if no args are given argc = 1 */
			check(argc - i > 3,"Usage: main -1 <waveform> <freq> <amplitude> ...");
			arg = argv[++i]; //Shift to the next arg
			/* Make sure arg is one of the valid waveform values[0-3] */
			if((rc = wave_atoi(arg)) >= 0){
				ch1.shape = (unsigned int)rc;
			
			}
			else{
				sentinel("Usage: main -1 [sine|square|saw|triang]....");
			}
			debug("ch1.shape: %s:%d",wave_names[ch1.shape],ch1.shape);

			arg = argv[++i];
			/* arg must evaluate to a positive number			 
			 *If arg doesn't contain any digits at the beginning,then strtol_ptr = arg is true and the test fails
			 */			
			if((rc = strtol(arg,&strtol_ptr,10)) >= 0 && (arg != strtol_ptr)){
				check(!errno,"");
				ch1.freq = rc;
			}
			else{
				//End the program
				sentinel("Frequency value is invalid");
			}
			debug("ch1.freq: %lu",ch1.freq);

			arg = argv[++i];
			if((rc = strtol(arg,&strtol_ptr,10)) >= 0 && (arg != strtol_ptr)){
				check(!errno,"");
				ch1.amp = rc;
			}
			else{
				sentinel("Amplitude value is invalid");
			}
			debug("ch1.amp: %lu",ch1.amp);
		}
		else if(strcmp(arg,"-2") == 0){
			/* There must be three or more arguments following '-2' */
			check(argc - i > 3,"Usage: main -2 <waveform> <freq> <amplitude>");
			arg = argv[++i];//Shift to the next argument

			if((rc = wave_atoi(arg)) >= 0){
				ch2.shape = rc;
			}
			else{
				sentinel("Usage: main -2 [sine|square|saw|triang]");
			}
			debug("ch2.shape: %s:%d",wave_names[ch2.shape],ch2.shape);

			arg = argv[++i];

			if((rc = strtol(arg,&strtol_ptr,10)) >= 0 && (arg != strtol_ptr)){
				check(!errno,"");
				ch2.freq = rc;
			}
			else{
				sentinel("Frequency value is invalid");
			}
			debug("ch2.freq: %lu",ch2.freq);

			arg = argv[++i];
			if((rc = strtol(arg,&strtol_ptr,10)) >= 0 && (arg != strtol_ptr)){
				check(!errno,"");
				ch2.amp = rc;
			}
			else{
				sentinel("Amplitude value is invalid");
			}
			debug("ch2.amp: %lu",ch2.amp);
		}
	}

	err = init();
	check_err("init");

	err = print_params();
	check_err("print_params");

	puts("\nChannel 1");
	print_wave_params(&ch1);
	puts("\nChannel 2");
	print_wave_params(&ch2);

	err = output_waves(&ch1,&ch2);
	check_err("output_waves");

	return 0;
error:
	if(hwparams)
		free(hwparams);
	return 1;

}


