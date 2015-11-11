#include "event.h"

static int fd;

int event_init(){
	return 1; //yes
}

int seteventsource(const char * path){
	//since out event handling globals are static, we need getters
	//and setters for some things like opening an /dev/input/* device
	fd = open(path, O_RDONLY);
	if(fd == -1) {
		perror("open");
		return -1;
	}
	return fd;
}

void printsourceinfo(){
	//i can't access the fd directly so i'll put my ioctl stuff here
	//and eventually refactor into a getname etc type of thing
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
	int r = 0;
	struct timeval tv = {.tv_sec = 0, .tv_usec = 0 };
	fd_set rfds;
	
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	select(fd+1, &rfds, NULL, NULL, &tv);
	
	if(FD_ISSET(fd, &rfds)){
		r = read(fd, event, sizeof(struct input_event));
	}
	
	return r;
}

int event_close(){
	//just close the event source for now
	close(fd);
	return 1;
}
