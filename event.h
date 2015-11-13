#ifndef H_EVENT
#define H_EVENT

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>

#define MAX_EVENT_FDS 256

//minor api regression, no explicit cleanup
//os will close the file descriptors, but it's lazy
int addeventsource(const char * path);
int pollevent(struct input_event * event);

#endif //H_EVENT
