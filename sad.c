#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <dirent.h>
#include <string.h>

#include "event.h"
#include "delay.h"
#include "event-dev-input.c"

#define VERSION "1.1"
#define DEFAULT_EVENT_DEVICE "/dev/input/event0"

//commands to invoke external programs
#define CMD_VOLUMEUP "amixer -q -- sset Master 1dB+"
#define CMD_VOLUMEDOWN "amixer -q -- sset Master 1dB-"

#define KEYBOARD_QUIT_ENABLED

char gpios[] = { //this is largely useless, it lists some promising
	//gpio pins on the raspberrypi v1 header
	4,17,27,22,23,24,25,18 }; //no useful alts

typedef enum { false, true } bool;

const char * cmdline_gen(const char * cmd, const char ** args, int nargs){
	//i want to pass a tailored command line to system()
	int size = 0, i = 0, cursor = 0;
	char * cmdline = NULL;

	//determine total size of string
	size += strlen(cmd);
	i = nargs;
	while(--i){
		size += strlen(args[i]);
	}
	size += nargs + 2; //1 nul terminator and a space after every argument

	//get memory or give up
	cmdline = malloc(size);
	if(cmdline == NULL) return NULL;

	//concatenate all the strings
	i = 0;
	while(cmd[i]) {
		cmdline[cursor++] = cmd[i++]; //copy command
	}
	cmdline[cursor++] = ' ';
	
	while(--nargs){
		i = 0;
		while((*args)[i]){
			cmdline[cursor++] = (*args)[i++];
		}
		cmdline[cursor++] = ' ';
		args++; //like args[t++] but without the extra variable
	}
	  
	return cmdline;
}

int main(int argc, char *argv[]){
	//handle some events, so i can make a lean gpio-keys event polling loop
	if (argc < 3) {
		printf("Usage:\n\tsad vlc socket_path library_path\n");
		return -1;
	}
	struct input_event event = { }, last = { };
	const char * cmdline = cmdline_gen("cvlc", (const char *[]){ "-Ioldrc", "--rc-unix",  argv[1], argv[2], "2>/dev/null", ">/dev/null" }, 6);
	int running = true; //set this to 0 if you'd like to exit the mainloop for any reason
	int rc; //some generic iterators and temporaries
	int vlcsock; //vlc socket handle
	struct sockaddr_un addr = { AF_UNIX, { } }; //default sockaddr
	pthread_t vlc;

	memcpy(addr.sun_path, argv[1], strlen(argv[1]));
	printf("Cmdline: %s\n", cmdline);
	pthread_create(&vlc, NULL, (void*(*)(void*))system, (void * restrict) cmdline);

	event_add_all_inputs();
	
	vlcsock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(vlcsock == -1) { perror("socket"); running = false; }

	do {
		rc = connect(vlcsock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
		if(rc != 0){
			perror("connect");
			running = false;
			printf("trying again in 1 second...\n");
			_delay_ms(1000);
		} else {
			running = true;
		}
	} while(rc != 0); //connection is needed for proper function

	printf("press q twice in 1s to quit.\n");
	write(vlcsock, "random on\n", 10); //enable shuffle
	write(vlcsock, "loop on\n", 8); //enable loop

	while(running){
		while(event_poll(&event)){
			switch(event.type){
				case EV_KEY:
					if(event.value == 0){
						//key is released
						break;
					}
					switch (event.code){
						case KEY_VOLUMEUP:
							system(CMD_VOLUMEUP);
							break;
						case KEY_VOLUMEDOWN:
							system(CMD_VOLUMEDOWN);
							break;
						//vlc control cases, apparently you can't feed a null byte to the socket
						//without screwing everything up, so that's why the write call truncates
						//the string constant like that
						case KEY_PLAYPAUSE:
							printf("Play pressed.\n");
							write(vlcsock, "pause\n", 6);
							break;
						case KEY_NEXTSONG:
							printf("Next Song.\n");
							write(vlcsock, "next\n", 5);
							break;
						case KEY_PREVIOUSSONG:
							printf("Prev Song.\n");
							write(vlcsock, "prev\n", 5);
							break;
						#ifdef KEYBOARD_QUIT_ENABLED
						case KEY_Q:
							//q twice exits
							if (event_compare(&event, &last) &&
									(event.time.tv_sec - last.time.tv_sec) < 1) {
								printf("Qutting...\n");
								write(vlcsock, "quit\n", 5);
								running = 0;
							} else {
								//store previous event
								event_copy(&event, &last);
							}
							break;
						#endif
						default:
							break;
					}
				default:
					break;
			}

		}
		_delay_ms(1000/60); //60hz polling
	}

	pthread_join(vlc, NULL);
	
	close(vlcsock);
	
	return 0;
}
