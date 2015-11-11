/* this file defines the api for fdready. it is essentially a wrapper for select
 * and simplifies the half dozen lines of code you would ordinarily need to check
 * if a given fd is ready to read or ready to write
 */
#ifndef _H_FDREADY_
#define _H_FDREADY_
#include <sys/select.h>

enum fdop_t {
	FDOP_READ = 0,
	FDOP_WRITE,
	FDOP_ERROR
};

int fdready(int fd, enum fdop_t operation);
#endif
