#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "event.h"
#include "delay.h"
#define DEFAULT_EVENT_DEVICE "/dev/input/event0"

//commands to invoke external programs
#define CMD_VOLUMEUP "amixer -q -- sset Master 1dB+"
#define CMD_VOLUMEDOWN "amixer -q -- sset Master 1dB-"
//--no-playlist-autostart
#define CMD_VLC "cvlc -Irc --rc-unix=\"/home/timothy/vlc.sock\" \"/home/timothy/library\" >/dev/null"

char gpios[] = { //this is largely useless, it lists some promising
	//gpio pins on the raspberrypi v1 header
	4,17,27,22,23,24,25,18 }; //no useful alts

typedef enum { false, true } bool;
typedef enum {
	SAD_PLAY,
	SAD_PAUSE,
	SAD_STOP,
	SAD_QUIT,
	SAD_WHATEVER
} sad_status_t;

sad_status_t status;

int main(){
	//handle some events, so i can make a lean gpio-keys event polling loop
	struct input_event event;
	int running = true; //set this to 0 if you'd like to exit the mainloop for any reason
	int rc; //some generic iterators and temporaries
	int vlcsock; //vlc socket handle
	struct sockaddr_un addr = { AF_UNIX, "/home/timothy/vlc.sock" }; //default sockaddr
	pthread_t vlc;

	pthread_create(&vlc, NULL, (void*(*)(void*))system, CMD_VLC);
	addeventsource(DEFAULT_EVENT_DEVICE);

	vlcsock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(vlcsock == -1) { perror("socket"); running = false; }

	rc = connect(vlcsock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
	if (rc != 0) { perror("connect"); running = false; }
	
	while(running){
		while(pollevent(&event)){
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
						case KEY_Q:
							printf("Qutting...\n");
							write(vlcsock, "quit\n", 5);
							running = 0;
							break;
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
