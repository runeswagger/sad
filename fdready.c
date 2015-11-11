/* this file provides the source code for fdready, a select wrapper. */
#include "fdready.h"

int fdready(int fd, enum fdop_t operation){
	if (fd < 0 || operation < FDOP_READ || operation > FDOP_ERROR) {
		//one of the arguments was bad
		return -1;
	}
	int nfds = fd + 1; //nfds is just 1 more than the highest numbered fd
	fd_set fds[3];
	struct timeval timeout = { .tv_sec = 0, .tv_usec = 0 };

	FD_ZERO(&fds[FDOP_READ]);
	FD_ZERO(&fds[FDOP_WRITE]);
	FD_ZERO(&fds[FDOP_ERROR]);

	FD_SET(fd, &fds[operation]);
	
	select(nfds, &fds[FDOP_READ], &fds[FDOP_WRITE], &fds[FDOP_ERROR], &timeout);

	return FD_ISSET(fd, &fds[operation]);
}
