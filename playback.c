
#define NDEBUG
#include <stdio.h>
#include <alsa/asoundlib.h>
#include "dbg.h"
#include "playback.h"

snd_pcm_t *device;
snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
snd_pcm_hw_params_t *hwparams;

int err;
const char *name = "hw:1,0";

/*For debugging purposes*/
int alsa_main(void){
	
	alsa_alloca();
	alsa_init();
	alsa_set_access(SND_PCM_ACCESS_RW_INTERLEAVED);
	unsigned int min,max;
	alsa_channels_minmax(&min,&max);
	debug("min: %u\tmax: %u",min,max);

	return 0;
}

/*For debugging purposes*/
void alsa_alloca(void){

	snd_pcm_hw_params_alloca(&hwparams);
	debug("hwparams = %p",hwparams);

}

/*Initialize PCM as a playback device*/

int alsa_init(void){
	
	//Open pcm device
	err = snd_pcm_open(&device,name,stream,0);
	check_err("snd_pcm_open");	//On error these functions return a negative number

	snd_pcm_hw_params_malloc(&hwparams);
	
	//Initialize the configuration space to default values
	err = snd_pcm_hw_params_any(device,hwparams);
	check_err("snd_pcm_hw_params_any");

	return 0;
error:
	return -1;

}

/*Set access type either INTERLEAVED or NON INTERLEAVED*/
int alsa_set_access(unsigned int access){
	
	//Check if the access type is available in the configuration space
	err = snd_pcm_hw_params_test_access(device,hwparams,access);
	check_err("Access type is not available in this configuration space");

	//Restrict the configuration space to have one access type
	err = snd_pcm_hw_params_set_access(device,hwparams,access);
	check_err("snd_pcm_hw_params_set_access");

	//Check to ensure the access type is set properly
	unsigned int get_access;
	//The next line assigns to get_access the integer that represents the access type
	snd_pcm_hw_params_get_access(hwparams,&get_access);
	debug("get_access: %d",get_access);
	check(get_access == access, "Access types don't match"); //If they don't match
	
	return access;
error:
	return -1;

}

int alsa_get_access(unsigned int *access){

	err = snd_pcm_hw_params_get_access(hwparams,access);
	check_err("snd_pcm_hw_params_get_access");

	return 0;
error:
	return -1;
}

int alsa_channels_minmax(unsigned int *min_channels,unsigned int *max_channels){
	
	err = snd_pcm_hw_params_get_channels_min(hwparams,min_channels);
	check_err("snd_pcm_hw_params_get_channels_min");
	err = snd_pcm_hw_params_get_channels_max(hwparams,max_channels);
	check_err("snd_pcm_hw_params_get_channels_max");

	return 0;
error:
	return -1;
}

int alsa_set_channels(unsigned int channels){

	//check if the given number of channels is available
	err = snd_pcm_hw_params_test_channels(device,hwparams,channels);
	check_err("The given channels count is not available in this configuration space");

	err = snd_pcm_hw_params_set_channels(device,hwparams,channels);
	check_err("snd_pcm_hw_params_set_channels");

	unsigned get_channels;
	snd_pcm_hw_params_get_channels(hwparams,&get_channels);
	debug("get_channels: %d",get_channels);
	check(get_channels == channels,"Channels count doesn't match");

	return channels;
error:
	return -1;
}

int alsa_get_channels(unsigned int *channels){

	err = snd_pcm_hw_params_get_channels(hwparams,channels);
	check_err("snd_pcm_hw_params_get_channels");

	return 0;
error:
	return -1;
}

int alsa_set_format(int format){

	//check if the given format is available
	err = snd_pcm_hw_params_test_format(device,hwparams,format);
	check_err("Format is not available in this configuration space");

	err = snd_pcm_hw_params_set_format(device,hwparams,format);
	check_err("snd_pcm_hw_params_set_format");

	int get_format;
	snd_pcm_hw_params_get_format(hwparams,&get_format);
	debug("get_format: %d",get_format);
	check(format == get_format,"Formats don't match");

	return format;
error:
	return -1;
}

int alsa_get_format(int *format){

	err = snd_pcm_hw_params_get_format(hwparams,format);
	check_err("snd_pcm_hw_params_get_format");

	return 0;
error:
	return -1;
}

int alsa_periods_minmax(unsigned int *min,unsigned int *max){

	err = snd_pcm_hw_params_get_periods_min(hwparams,min,NULL);
	check_err("snd_pcm_hw_params_get_periods_min");

	err = snd_pcm_hw_params_get_periods_max(hwparams,max,NULL);
	check_err("snd_pcm_hw_params_get_periods_max");

	return 0;
error:
	return -1;

}

int alsa_set_periods_exact(unsigned int periods){

	const int exact = 0;
	err = snd_pcm_hw_params_test_periods(device,hwparams,periods,exact);
	check_err("The given number of periods is not available");

	err = snd_pcm_hw_params_set_periods(device,hwparams,periods,exact);
	check_err("snd_pcm_hw_params_set_periods");

	unsigned int get_periods;
	snd_pcm_hw_params_get_periods(hwparams,&get_periods,NULL);
	debug("Periods: %u",get_periods);
	check(periods == get_periods,"Period counts don't match");

	return periods;
error:
	return -1;
}

int alsa_set_periods_near(unsigned int periods){

	//real_period will be used to hold the value of the number of periods that's actually used
	unsigned int real_periods = periods;	//Initially equals periods
	/*
	 * Sets the number of periods to the nearest available value in the configuration space
	 * and stores this value in real_periods.
	 */
	err = snd_pcm_hw_params_set_periods_near(device,hwparams,&real_periods,NULL);
	debug("real_periods: %u",real_periods);
	check_err("snd_pcm_hw_params_set_periods_near");

	//If the number of periods set in the configuration space doesn't match the desired one
	if(real_periods != periods){
		log_info("Periods is set automatically to: %u",real_periods);
	}

	unsigned int get_periods;
	snd_pcm_hw_params_get_periods(hwparams,&get_periods,NULL);
	debug("get_periods: %u",get_periods);
	check(get_periods == real_periods,"Cannot set this period count");

	return real_periods;
error:
	return -1;
}

int alsa_get_periods(unsigned int *periods){

	err = snd_pcm_hw_params_get_periods(hwparams,periods,NULL);
	check_err("snd_pcm_hw_params_get_periods");

	return 0;
error:
	return -1;
}

int alsa_rate_minmax(unsigned int *min,unsigned int *max){

	err = snd_pcm_hw_params_get_rate_min(hwparams,min,NULL);
	check_err("snd_pcm_hw_params_get_rate_min");

	err = snd_pcm_hw_params_get_rate_max(hwparams,max,NULL);
	check_err("snd_pcm_hw_params_get_rate_max");

	return 0;
error: 
	return -1;
}

int alsa_set_rate_exact(unsigned int rate){

	const int exact = 0;
	err = snd_pcm_hw_params_test_rate(device,hwparams,rate,exact);
	check_err("The give rate is not available");

	err = snd_pcm_hw_params_set_rate(device,hwparams,rate,exact);
	check_err("snd_pcm_hw_params_set_rate");

	unsigned int get_rate;
	snd_pcm_hw_params_get_rate(hwparams,&get_rate,NULL);
	debug("get_rate: %u",get_rate);
	check(rate == get_rate,"Sample rates don't match");

	return 0;
error:
	return -1;
}

int alsa_set_rate_near(unsigned int rate){

	unsigned int real_rate = rate;
	err = snd_pcm_hw_params_set_rate_near(device,hwparams,&real_rate,NULL);
	debug("real_rate: %u",real_rate);
	check_err("snd_pcm_hw_params_set_rate_near");

	if(real_rate != rate){
		log_info("Sample rate is set automatically to: %u",real_rate);
	}

	unsigned int get_rate;
	snd_pcm_hw_params_get_rate(hwparams,&get_rate,NULL);
	debug("get_rate: %u",get_rate);
	check(get_rate == real_rate,"Cannot set this sample rate value");

	return real_rate;
error:
	return -1;
}

int alsa_get_rate(unsigned int *rate){

	err = snd_pcm_hw_params_get_rate(hwparams,rate,NULL);
	check_err("snd_pcm_hw_params_get_rate");

	return 0;
error:
	return -1;
}

int alsa_period_size_minmax(unsigned long *min,unsigned long *max){

	//Extract min and max period size in frames
	err = snd_pcm_hw_params_get_period_size_min(hwparams,min,NULL);
	check_err("snd_pcm_hw_params_get_period_size_min");

	err = snd_pcm_hw_params_get_period_size_max(hwparams,max,NULL);
	check_err("snd_pcm_hw_params_get_period_size_max");

	return 0;
error:
	return -1;
}

long alsa_set_period_size_exact(unsigned long periodsize){

	//Set period size in unit of frames
	const int exact = 0;
	err = snd_pcm_hw_params_test_period_size(device,hwparams,periodsize,exact);
	check_err("Period size is not available in this configuration space");

	err = snd_pcm_hw_params_set_period_size(device,hwparams,periodsize,exact);
	check_err("snd_pcm_hw_params_set_period_size");

	unsigned long get_periodsize;
	snd_pcm_hw_params_get_period_size(hwparams,&get_periodsize,NULL);
	debug("get_periodsize: %lu",get_periodsize);
	check(get_periodsize == periodsize,"Period sizes don't match");

	return get_periodsize;
error:
	return -1;
}

long alsa_set_period_size_near(unsigned long periodsize){

	//Set period size in unit of frames
	unsigned long real_periodsize = periodsize;

	err = snd_pcm_hw_params_set_period_size_near(device,hwparams,&real_periodsize,NULL);
	debug("real_periodsize: %lu",real_periodsize);
	check_err("snd_pcm_hw_params_set_period_size_near");

	if(real_periodsize != periodsize){
		log_info("Period size has been set to: %lu",real_periodsize);
	}

	unsigned long get_periodsize;
	snd_pcm_hw_params_get_period_size(hwparams,&get_periodsize,NULL);
	debug("get_periodsize: %lu",get_periodsize);
	check(get_periodsize == real_periodsize,"Cannot set the given period size");

	return real_periodsize;
error:
	return -1;
}

int alsa_get_period_size(unsigned long *periodsize){

	err = snd_pcm_hw_params_get_period_size(hwparams,periodsize,NULL);
	check_err("snd_pcm_hw_params_get_period_size");
	
	return 0;
error:
	return -1;
}

int alsa_buffer_size_minmax(unsigned long *min,unsigned long *max){

	/*Extract buffer size in unit of frames*/
	err = snd_pcm_hw_params_get_buffer_size_min(hwparams,min);
	check_err("snd_pcm_hw_params_get_buffer_size_min");

	err = snd_pcm_hw_params_get_buffer_size_max(hwparams,max);
	check_err("snd_pcm_hw_params_get_buffer_size_max");

	return 0;
error:
	return -1;
}

long alsa_set_buffer_size_exact(unsigned long buffersize){

	/*Set buffer size in frames*/
	err = snd_pcm_hw_params_test_buffer_size(device,hwparams,buffersize);
	check_err("Buffer size is not available in this configuration space");

	err = snd_pcm_hw_params_set_buffer_size(device,hwparams,buffersize);
	check_err("snd_pcm_hw_params_set_buffer_size");

	unsigned long get_buffersize;
	snd_pcm_hw_params_get_buffer_size(hwparams,&get_buffersize);
	debug("get_buffersize: %lu",get_buffersize);
	check(get_buffersize == buffersize,"Buffer sizes don't match");

	return get_buffersize;
error:
	return -1;
}

long alsa_set_buffer_size_near(unsigned long buffersize){

	/*Set buffer size to an approximate value in unit frames*/
	unsigned long real_buffersize = buffersize;
	err = snd_pcm_hw_params_set_buffer_size_near(device,hwparams,&real_buffersize);
	debug("real_buffersize: %lu",real_buffersize);
	check_err("snd_pcm_hw_params_set_buffer_size_near");

	if(real_buffersize != buffersize){
		log_info("Buffer size has been set to: %lu",real_buffersize);
	}

	unsigned long get_buffersize;
	snd_pcm_hw_params_get_buffer_size(hwparams,&get_buffersize);
	debug("get_buffersize: %lu",get_buffersize);
	check(get_buffersize == real_buffersize,"Cannot set buffer size");

	return real_buffersize;
error:
	return -1;
}

int alsa_get_buffer_size(unsigned long *buffersize){

	err = snd_pcm_hw_params_get_buffer_size(hwparams,buffersize);
	check_err("snd_pcm_hw_params_get_buffer_size");
	
	return 0;
error:
	return -1;
}

int alsa_set_params(void){

	/*Set the hw paramaters in the configuration space*/
	err = snd_pcm_hw_params(device,hwparams);
	check_err("Cannot set HW parameters for PCM");

	return 0;
error:
	return -1;
}

int alsa_writei(void *data,unsigned long frames){

	int access;
	int rc;
	err = snd_pcm_hw_params_get_access(hwparams,&access);
	check_err("snd_pcm_hw_params_get_access");
	check(access == ACCESS_INTERLEAVED,"Invalid access type");

	rc = snd_pcm_writei(device,data,frames);
	debug("Number of written frames: %d",rc);
	if(rc < frames){
		log_warn("Buffer Underrun after writing %d frames",rc);
		return 1;
	}

	return 0;
error:
	return -1;
}
		
