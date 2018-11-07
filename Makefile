
main: main.o playback.o wave.o
	cc -Wall -g main.o playback.o wave.o -lasound -lm -o main

main.o: main.c
	cc -Wall -g -c main.c

playback.o: playback.c
	cc -Wall -g -c playback.c

wave.o: wave.c
	cc -Wall -g -c wave.c

touch:
	touch main.o playback.o wave.o
	
clear:
	-rm *.o main
