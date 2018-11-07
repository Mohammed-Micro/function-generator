#ifndef _wave_h
#define _wave_h

/* Open the PCM device and set its HW paramaters*/
int init(void);

/* Prints the HW parameters for a PCM*/
int print_params(void);

/* Prints the min and max HW parameters in the configuration space*/
int print_minmax(void);

/*Genearate a sine wave with the given frequency and amplitude*/
char *generate_sine(unsigned long,unsigned long,char **);


char *generate_saw(unsigned long,unsigned long,char **);

char *generate_square(unsigned long,unsigned long,char **);

/* Combines The data of two buffers,representing two channels,and then
 * Write them to the ring buffer of the PCM device
 */
char *play(const char *,const char *);

#endif
