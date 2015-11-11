#include "delay.h"

int _delay_ms(int ms){
	//call nanosleep for specified duration
	struct timespec t = { .tv_sec = 0, .tv_nsec = 0 };

	//initialize the timespec to requested delay
	while(ms >= 1000) { //allow delays of more than 1000 ms
		t.tv_sec++;
		ms -= 1000;
	}

	//multiply remainder of ms by 1000 to get number of ns
	t.tv_nsec = 1000000 * ms;
	
	int rv = 0;

	do {
		rv = nanosleep(&t,&t); //request delay of t and record elapsed delay in t
		//check the return value and possibly abort this function
		if(rv == -1 && errno == EINVAL) {
			//requested time is an invalid value, return
			return 0;
		}
	} while(rv); //nanosleep returns 0 when the time has passed

	return 0;
}
