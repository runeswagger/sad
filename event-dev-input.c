/* I find myself needing this function often enough to
 * remove it from sad.c but don't want to do directory parsing
 * in event.c because event.c is supposed to be lightweight
 * so programs that want to open every input device can
 * include this c file */
#include <dirent.h>
#include <string.h>

#include "event.h"

void event_add_all_inputs(){
	//scan /dev/input and add all the input devices
	DIR *d;
	struct dirent *e;
	char * pathbuf;
	int pathsize = 0, pathbufsize = 0;
	
	d = opendir("/dev/input");
	if (d == NULL) perror("opendir");

	while ((e = readdir(d)) != NULL){
		if(e->d_type == DT_CHR){
			pathsize = strlen("/dev/input/") + strlen(e->d_name); //size of path
			if (pathsize > pathbufsize) {
				//alloca usually just decrements the stack pointer
				//so calling alloca on the size difference
				//will typically just push pathbuf farther up on the stack
				pathbuf = alloca(pathsize - pathbufsize);
			}
			strcpy(pathbuf, "/dev/input/");
			strcat(pathbuf, e->d_name);
			printf("%s\n", pathbuf);
			event_add_source(pathbuf);
		}
	}

	event_get_name(0);
	closedir(d);
}
