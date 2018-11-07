#ifndef _playback_h
#define _playback_h

#include <alsa/asoundlib.h>

#define ACCESS_INTERLEAVED SND_PCM_ACCESS_RW_INTERLEAVED
#define ACCESS_NONINTERLEAVED SND_PCM_ACCESS_RW_NONINTERLEAVED
#define FORMAT_S16_LE SND_PCM_FORMAT_S16_LE

/*If err is negative the macro prints an error message and jumps to the error block in the calling function.*/
#define check_err(T) check(err >= 0,T ": %d",err)

extern snd_pcm_t *device;
extern snd_pcm_hw_params_t *hwparams;
extern snd_pcm_stream_t stream;
extern const char *name;

extern int alsa_main(void);//For debugging
extern void alsa_alloca(void);//For debugging

/*Open the pcm device and initialize the hw_params struct to default values*/
extern int alsa_init(void);

/*
 * Set the access type for the pcm device
 * Can be ACCESS_INTERLEAVED for example
 */
extern int alsa_set_access(unsigned int);

extern int alsa_get_access(unsigned int *);

/*Extracts the min and max allowed number of channels for the given configuration space
 * and puts them in the locations pointed to by the given pointers
 */
extern int alsa_channels_minmax(unsigned int *, unsigned int *);

//Set number of channels
extern int alsa_set_channels(unsigned int);

extern int alsa_get_channels(unsigned int *);

//Set format of the sample data
extern int alsa_set_format(int);

extern int alsa_get_format(int *);

/*Extracts the min and max allowed number of periods
 * for the given configuration space.
 */
extern int alsa_periods_minmax(unsigned int *, unsigned int *);

/*This functions trys to set the number of periods to the given value
 * ,and fails if the value is not supported
 */
extern int alsa_set_periods_exact(unsigned int);

/*This function trys to set the number of periods to the given value
 * ,if the value is not supported it's set to the nearest supported value by
 * the given configuration space.
 */
extern int alsa_set_periods_near(unsigned int);

extern int alsa_get_periods(unsigned int *);

//Extract the min and max valid sample rates
extern int alsa_rate_minmax(unsigned int *, unsigned int *);

//Set the sample rate,fails if not supported
extern int alsa_set_rate_exact(unsigned int);

//Set the sample rate to the nearest valid value to the desired one
extern int alsa_set_rate_near(unsigned int);

extern int alsa_get_rate(unsigned int *);

//Extracts the min and max period sizes in frames
extern int alsa_period_size_minmax(unsigned long *, unsigned long *);

//Set the period size value in frames,fails if the desired value is not in the configuration space
extern long alsa_set_period_size_exact(unsigned long);

//Set the period size in frames to the nearest valid value
extern long alsa_set_period_size_near(unsigned long);

extern int alsa_get_period_size(unsigned long *);

//Extracts the min and max ring buffer size in frames
extern int alsa_buffer_size_minmax(unsigned long *, unsigned long *);

//Set the the buffer size,fails if the given value doesn't exist in the configuration space
/*Note:You can set either the buffer size or the period size,the other parameter is set automatically
 * according to the following relation
 * buffer size = period size * periods
 */
extern long alsa_set_buffer_size_exact(unsigned long);

extern long alsa_set_buffer_size_near(unsigned long);

extern int alsa_get_buffer_size(unsigned long *);

//Applies the given parameters in the configuration space to the PCM device
extern int alsa_set_params(void);

/*Write frames from buffer to PCM
 * Returns
 *  0 -> if all frames has been written
 *  1 -> if less frames than specified has been written
 * -1 -> on error
 */
 extern int alsa_writei(void *,unsigned long);

#endif
