#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include "dbg.h"
#include "playback.h"
#include "wave.h"

static int err;
int main(int argc,char **argv){

	char *data1 = NULL, *data2 = NULL;
	char *ptr = NULL, *buffer = NULL;

	err = init();
	check_err("init");

	err = print_params();
	check_err("print_params");

	while(1){
		ptr = generate_square(1000,30000,&data1);
		check(ptr,"generate_square:1");

		ptr = generate_saw(1000,30000,&data2);
		check(ptr,"generate_sine:2");

		buffer = play(data1,data2);
		check(buffer,"play");
	}

	return 0;
error:
	if(data1){
		free(data1);
	}
	if(data2){
		free(data2);
	}
	if(buffer){
		free(buffer);
	}
	if(hwparams){
		free(hwparams);
	}
	return 1;
}

