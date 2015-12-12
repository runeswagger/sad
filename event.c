#include "event.h"

//i think these get zero initialized in C
static int fds[MAX_EVENT_FDS];
static int nfds;
static char info[EVENT_STRING_BUF_SIZE];

int event_debug_dump_fds(){
	int i = 0;
	printf("[");
	while (i < nfds){
		printf(" %i ", fds[i]);
	}
	printf("]\n");
	return i;
}

int event_fd_compare(int fd1, int fd2){
	//test if both fds refer to the same file
	struct stat stat1, stat2;

	if(fstat(fd1, &stat1) < 0) return -1;
	if(fstat(fd2, &stat2) < 0) return -1;

	return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

int event_index_from_fd(int fd){
	//search fds for a given fd
	int index = 0;

	while(index < nfds && fds[index] != fd) index++;

	return (index == nfds) ? -1 : index; //-1 if array search and no match found
}

int event_add_fd(int fd){
	//add an open fd
	if(nfds >= MAX_EVENT_FDS || fd == -1) return -1;
	int index = event_index_from_fd(fd); //this searches fds and returns -1 if no match
	
	if (index == -1){
		fds[nfds] = fd;
		nfds++;
	} 
	
	return (index == -1) ? -1 : fd;
}

int event_remove_fd(int fd){
	int index = event_index_from_fd(fd);

	if (index != -1) fds[index] = -1;

	return (index != -1);
}

int event_add_source(const char * path){
	//open path and add the fd to fds
	int fd = open(path, O_RDONLY);

	int i = 0;

	while(i < nfds){
		//break on match
		if(event_fd_compare(fd, fds[i])){
			break;
		}
		i++;
	}

	return (i < nfds) ? /* match found when comparing fds */ -1 : /* new file */ event_add_fd(fd);
}

char * event_get_name(int fd){
	//get name of device
	info[ioctl(fds[event_index_from_fd(fd)], EVIOCGNAME(sizeof(info)), info)] = 0; //append null terminator
	return info;
}

char * event_get_phys(int fd){
	//get physical location of dev file
	info[ioctl(fds[event_index_from_fd(fd)], EVIOCGPHYS(sizeof(info)), info)] = 0;
	return info;
}

char * event_get_info(int fd){
	//get unique identifier
	info[ioctl(fds[event_index_from_fd(fd)], EVIOCGUNIQ(sizeof(info)), info)] = 0;
	return info;
}

struct input_id * event_get_id(int fd){
	static struct input_id id;

	ioctl(fds[event_index_from_fd(fd)], EVIOCGID, &id);

	return &id;
}

int event_poll(struct input_event * event){
	//get next event
	if(!nfds) return 0; //no sources opened
	int maxfd = fds[nfds], i = 0;
	struct timeval tv = {.tv_sec = 0, .tv_usec = 0 };
	fd_set rfds;

	FD_ZERO(&rfds);
	while(i < nfds){
		FD_SET(fds[i], &rfds);
		maxfd = fds[i] > maxfd ? fds[i] : maxfd; //update max fd
		i++;
	}

	select(maxfd+1, &rfds, NULL, NULL, &tv);

	i = nfds;
	do {
		--i;
		if(FD_ISSET(fds[i], &rfds)){ //always breaks on the newest one
			read(fds[i], event, sizeof(struct input_event));
			break;
		}
	} while(i);
	
	return i;
}

int event_is_newer(struct input_event *a, struct input_event *b){
	int newer;
	if(a->time.tv_sec > b->time.tv_sec && a->time.tv_usec > b->time.tv_usec){
		newer = 1;
	} else if (a->time.tv_sec == b->time.tv_sec && a->time.tv_usec == b->time.tv_usec){
		newer = 0;
	} else {
		newer = -1;
	}

	return newer;
}

int event_compare(struct input_event *a, struct input_event *b){
	return ((a->type == b->type)&&(a->code == b->code)&&(a->value == b->value));
}

//C99 is iffy on the inline usage, but this prototype will mean that event.o contains
//the real version of event_copy in case the compiler can't inline it for some reason
extern struct input_event* event_copy(struct input_event *src, struct input_event *dest);

inline struct input_event* event_copy(struct input_event *src, struct input_event *dest){
	return memcpy(dest, src, sizeof(struct input_event));
}

	

