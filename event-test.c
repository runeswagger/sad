/* print out events */
/* a test for expanding the event library */
#include "event.h"
#include "delay.h"

void printsourcenames();

int main(int argc, char *argv[]){
	//poll for and print events
	int delay = argc > 1 ? atoi(&argv[1][2]) : 1000/60; //60fps
	struct input_event event;
	int running = 1;
	
	event_add_source("/dev/input/event0");	
	event_get_name(0);
	
	while(running){
		while(event_poll(&event)){
			printf("Type: %i\nCode: %i\nValue: %i\n\n", event.type, event.code, event.value);
			if(event.type == EV_KEY && event.value == 1 && event.code == KEY_Q) running = 0; //quit program
			_delay_ms(delay);
		}
	}
	
	return 0;
}
