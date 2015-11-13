#include "event.h"

//i think these get zero initialized in C
static int fds[MAX_EVENT_FDS];
static int nfds;
	 
int addeventsource(const char * path){
	//we're moving to an aggregating event handling, so we keep a list of fds
	if(nfds >= MAX_EVENT_FDS) return -1; //TODO: ensure standard convention for error returns	
	fds[nfds] = open(path, O_RDONLY);

	if(fds[nfds] != -1) {
		nfds++;
		return fds[nfds-1];
	} else {
		perror("open");
		return -1;
	} 
}

void printsourceinfo(){
	//i can't access the fd directly so i'll put my ioctl stuff here
	//and eventually refactor into a getname etc type of thing
	int fd = fds[nfds-1];
	struct input_id id;
	char name[255]; //device name
	char phys[255]; //physical name
	char uid[255]; //unique identifier
	
	//get id of device
	ioctl(fd, EVIOCGID, &id); //get id

	//get name of device
	name[ioctl(fd, EVIOCGNAME(sizeof(name)), name)] = 0;
	printf("Name: %s\n", name);

	//get physical location of dev file
	phys[ioctl(fd, EVIOCGPHYS(sizeof(phys)), phys)] = 0;
	printf("Phys: %s\n", phys);

	//get unique identifier
	uid[ioctl(fd, EVIOCGUNIQ(sizeof(uid)), uid)] = 0;
	printf("UID: %s\n", uid);
}

int pollevent(struct input_event * event){
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
