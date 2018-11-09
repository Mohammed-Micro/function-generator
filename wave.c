#define NDEBUG
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbg.h"
#include "playback.h"
#include "wave.h"

#define PI 3.14159
#define BITS_PER_BYTE 8	

#define _sgn(X) (X>0.0)?1:-1

static int err = 0;

int init(void){

	unsigned int access = ACCESS_INTERLEAVED;
	unsigned int channels = 2; //Stereo stream
	unsigned int format = FORMAT_S16_LE;
	unsigned int rate = 96000;
	unsigned int periods = 2;
	unsigned long periodsize = 2048; //Hardcoded

	alsa_init();

	err = print_minmax();
	check_err("print_minmax");

	err = alsa_set_access(access);
	check_err("alsa_set_access");

	err = alsa_set_channels(channels);
	check_err("alsa_set_channels");

	err = alsa_set_format(format);
	check_err("alsa_set_format");

	err = alsa_set_rate_near(rate);
	check_err("alsa_set_rate_near");

	err = alsa_set_periods_near(periods);
	check_err("alas_set_periods_near");

	err = alsa_set_period_size_near(periodsize);
	check_err("alsa_set_period_size_near");

	err = alsa_set_params();
	check_err("asla_set_params");

	return 0;
error:
	return -1;
}

int print_params(void){

	unsigned int access;
	int format;
	unsigned int channels;
	unsigned int rate;
	unsigned int periods;
	unsigned long periodsize;
	unsigned long buffersize;

	err = alsa_get_access(&access);
	check_err("alsa_get_access");

	err = alsa_get_format(&format);
	check_err("alsa_get_format");

	err = alsa_get_channels(&channels);
	check_err("alsa_get_channels");

	err = alsa_get_rate(&rate);
	check_err("alsa_get_rate");

	err = alsa_get_periods(&periods);
	check_err("asla_get_periods");

	err = alsa_get_period_size(&periodsize);
	check_err("alsa_get_period_size");

	err = alsa_get_buffer_size(&buffersize);
	check_err("alsa_get_buffer_size");

	puts("--------HW parameters-----------");
	printf("Access type: %u\n",access);
	printf("Format: %u\n",format);
	printf("Number of channels: %u\n",channels);
	printf("Sample rate: %u\n",rate);
	printf("Periods per buffer: %u\n",periods);
	printf("Period size in frames: %lu\n",periodsize);
	printf("Buffer size in frame: %lu\n",buffersize);

	return 0;
error:
	return -1;

}

int print_minmax(void){

	unsigned int imin,imax;
	unsigned long lmin,lmax;

	puts("----------Min and Max parameter values----------");

	err = alsa_channels_minmax(&imin,&imax);
	check_err("alsa_channels_minmax");
	printf("Number of channels\n\t\tmin:%u\tmax: %u\n",imin,imax);

	err = alsa_rate_minmax(&imin,&imax);
	check_err("alsa_rate_minmax");
	printf("Sample rate\n\t\tmin: %u\tmax: %u\n",imin,imax);

	err = alsa_periods_minmax(&imin,&imax);
	check_err("alsa_periods_minmax");
	printf("Number of periods\n\t\tmin: %u\tmax: %u\n",imin,imax);

	err = alsa_period_size_minmax(&lmin,&lmax);
	check_err("alsa_period_size_minmax");
	printf("Period size in frames\n\t\tmin: %lu\tmax: %lu\n",lmin,lmax);

	err = alsa_buffer_size_minmax(&lmin,&lmax);
	check_err("alsa_buffer_size_minmax");
	printf("Buffer size in frames\n\t\tmin: %lu\tmax: %lu\n",lmin,lmax);

	return 0;
error:
	return -1;
}	

char *generate_sine(unsigned long freq,unsigned long amp,char **p_buffer){

	unsigned int rate;
	unsigned long periodsize;

	err = alsa_get_rate(&rate);
	debug("rate: %u",rate);
	check_err("alsa_get_rate");

	err = alsa_get_period_size(&periodsize);
	debug("periodsize: %lu",periodsize);
	check_err("alsa_get_period_size");

	check(rate >= 2 * freq,"Sample rate must be at least two times greater than the wave frequency");
	
	char *buffer = *p_buffer;
	/*If buffer is NULL,allocate some memory and let it point to that memory.*/
	if(!buffer){
		buffer = (char*) malloc(periodsize * sizeof(short));
		check_mem(buffer);
		*p_buffer = buffer;
	}

	unsigned long frames;
	/*Declared static so that the next write to buffer starts from the point at which the previous write ended.*/
	static unsigned long n = 0;
	/* Writing samples takes place at specific points of time that are evenly spaced.
	 * t equals an integer 'n' times the period of sampling
	 */
	float t;

	for(frames = 0;frames < periodsize;frames++){
		t = (float)n / (float) rate;
		debug("t: %f\tn: %lu",t,n);
		float fsample = sinf(2.0 * PI * (float)freq * t);
		short sample = fsample * amp;

		//This part works only for S16_LE
		buffer[2*frames + 0] = sample & 0x00ff;//This will make the MSB equals zero
		buffer[2*frames + 1] = (sample & 0xff00) >> BITS_PER_BYTE;

		/*Setting an upper bound to n,to prevent overflow*/
		if(++n == rate){
			n = 0;
			debug("Resetting n..........");
		}
	}

	return buffer;
error:
	return NULL;
}

char *generate_square(unsigned long freq, unsigned long amp, char **p_buffer){
	unsigned int rate;
	unsigned int periodsize;

	err = alsa_get_rate(&rate);
	debug("rate: %u", rate);
	check_err("alsa_get_rate");

	err = alsa_get_period_size(&periodsize);
	debug("periodsize: %lu",periodsize);
	check_err("alsa_get_rate");

	err = alsa_get_period_size(&periodsize);
	debug("periodsize: %lu",periodsize);
	check_err("alsa_get_period_size");

	check(rate >= 2 * freq,"Sample rate must be at least two times greater than the wave frequency");

	char *buffer = *p_buffer;
	/*If buffer is NULL,allocate some memory and let it point to the memory.*/
	if(!buffer){
		buffer = (char*) malloc(periodsize * sizeof(short));
		check_mem(buffer);
		*p_buffer = buffer;
	}

	unsigned long frames;
	/*Declared static so that the next write to the buffer starts from the point at which the previous write ended.*/
	static unsigned long n = 0;
	/* Writing samples takes place at specific points of time that are evenly spaced.
	 * t equals an integer 'n' times the period of sampling
	 */
	float t;

	for(frames = 0;frames < periodsize;frames++){
		t = (float)n / (float) rate;
		debug("t: %f\tn: %lu",t,n);
	
		short sample = amp * _sgn(modf((double)freq * (double)t,NULL) - .5);

		//This part works only for S16_LE
		buffer[2*frames + 0] = sample & 0x00ff;//This will make the MSB equal to zero
		buffer[2*frames + 1] = (sample & 0xff00) >> BITS_PER_BYTE;
		/*Setting an upper bound to n,to prevent overflow*/
		if(++n == rate){
			n = 0;
			debug("Resetting n........");
		}
	}

	return buffer;
error:
	return NULL;
}

char *generate_saw(unsigned long freq,unsigned long amp,char **p_buffer){
	unsigned int rate;
	unsigned long periodsize;

	err = alsa_get_rate(&rate);
	debug("rate: %u",rate);
	check_err("alsa_get_rate");

	err = alsa_get_period_size(&periodsize);
	debug("periodsize: %lu",periodsize);
	check_err("alsa_get_period_size");

	check(rate >= 2 * freq,"Sample rate must be at least two times greater than wave frequency");

	char *buffer = *p_buffer;
	/*If buffer is NULL,allocate some memory and let it point to that memory.*/
	if(!buffer){
		buffer = (char*) malloc(periodsize * sizeof(short));
		check_mem(buffer);
		*p_buffer = buffer;
	}

	unsigned long frames;
	/*Declared static so that the next write to buffer starts from the point at which the previous write ended.*/
	static unsigned long n = 0;
	/*Writing samples takes place at specific points of time that are evenly spaced.
	 * t equals an integer 'n' times the period of sampling
	 */
	float t;

	for(frames = 0;frames < periodsize;frames++){
		t = (float)n / (float)rate;
		debug("t: %f\tn: %lu",t,n);
		float fsample = modf((float)freq * t,NULL) - .5f;
		short sample = fsample * amp * 2;

		//This part works only for S16_LE
		buffer[2*frames + 0] = sample & 0x00ff;//This will make the MSB equal to zero
		buffer[2*frames + 1] = (sample & 0xff00) >> BITS_PER_BYTE;

		/*Setting an upper bound to n,to prevent overflow*/
		if(++n == rate){
			n = 0;
			debug("Resetting n.........");
		}
	}
	return buffer;
error:
	return NULL;
}

char *play(const char *ch1_buffer,const char *ch2_buffer){

	unsigned long periodsize;
	unsigned int channels;
	
	err = alsa_get_period_size(&periodsize);
	debug("periodsize: %lu",periodsize);
	check_err("alsa_get_period_size");

	err = alsa_get_channels(&channels);
	debug("channels: %u",channels);
	check_err("alsa_get_channels");

	check(ch1_buffer,"ch1_buffer is NULL");
	check(ch2_buffer,"ch2_buffer is NULL");

	/*buffer_length is the size in bytes of the memory pointed to by buffer below,
	 * not the length of the PCM buffer
	 */
	unsigned long buffer_length = channels * periodsize * sizeof(short);

	/*buffer is declared with static storage class,
	 * because allocating memory every time this function is called
	 * affects greatly the performance of the program
	 */
	/* Can't write {static char *buffer = malloc(....);}.
	 * Since static variables must be initialized with a constant expression
	 */
	static char *buffer = NULL; 
	if(!buffer){
		buffer = (char*) malloc(buffer_length);
		check_mem(buffer);
	}

	int i,j;
	for(i = 0,j = 0;j < buffer_length;i++){

		buffer[j++] = ch1_buffer[2*i + 0];
		buffer[j++] = ch1_buffer[2*i + 1];
		buffer[j++] = ch2_buffer[2*i + 0];
		buffer[j++] = ch2_buffer[2*i + 1];
	}

	err = alsa_writei(buffer,periodsize);
	check_err("alsa_writei");

	return buffer;
error:
	return NULL;
}

