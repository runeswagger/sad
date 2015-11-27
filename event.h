#ifndef H_EVENT
#define H_EVENT

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/stat.h>

#define MAX_EVENT_FDS 256

//minor api regression, no explicit cleanup
//os will close the file descriptors, but it's lazy
int event_add_fd(int fd);
int event_remove_fd(int fd);
int event_add_source(const char * path);
int event_poll(struct input_event * event);

//very bad convienience functions
void event_get_name(int fd);
void event_get_phys(int fd);

//debug functions
int event_debug_dump_fds();

#endif //H_EVENT
