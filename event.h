#ifndef H_EVENT
#define H_EVENT

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>

int event_init();
int seteventsource(const char * path);
int pollevent(struct input_event * event);
int event_close();

#endif //H_EVENT
