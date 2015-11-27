#include "event.h"

//i think these get zero initialized in C
static int fds[MAX_EVENT_FDS];
static int nfds;

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
	} else {
		printf("duplicate!\n");
	}
	
	return (index == -1) ? -1 : fd;
}

int event_remove_fd(int fd){
	int index = event_index_from_fd(fd);

	if (index != -1) fds[index] = -1;

	return (index != -1);
}

int event_add_source(const char * path){
	//we're moving to an aggregating event handling, so we keep a list of fds
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

void event_get_name(int fd){
	//get name of device
	char name[255];
	int i = 0;
	
	while(i < nfds){
		name[ioctl(fds[i], EVIOCGNAME(sizeof(name)), name)] = 0; //append null terminator
		printf("Name: %s\n", name);
		i++;
	}
}

void event_get_phys(int fd){
	//get physical location of dev file
	char phys[255];
	int i = 0;

	while(i < nfds){
		phys[ioctl(fds[i], EVIOCGPHYS(sizeof(phys)), phys)] = 0;
		printf("Phys: %s\n", phys);
		i++;
	}
}

void event_get_info(){
	//i can't access the fd directly so i'll put my ioctl stuff here
	//and eventually refactor into a getname etc type of thing
	int fd = fds[nfds-1];
	struct input_id id;
	char uid[255]; //unique identifier
	
	//get id of device
	ioctl(fd, EVIOCGID, &id); //get id


	//get unique identifier
	uid[ioctl(fd, EVIOCGUNIQ(sizeof(uid)), uid)] = 0;
	printf("UID: %s\n", uid);
}

int event_poll(struct input_event * event){
	//get next event
	if(!nfds) return 0; //no sources opened
	int r = 0, maxfd = fds[nfds], i = 0;
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
			r = read(fds[i], event, sizeof(struct input_event));
			break;
		}
	} while(i);
	
	return r;
}
