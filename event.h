#ifndef H_EVENT
#define H_EVENT
/** \file event.h
 * compile in code snippets for handling evdev
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>
#include <sys/stat.h>

#define MAX_EVENT_FDS 256

/** \brief adds an fd to fd pool
 *  \param fd fd to add
 *  \return fd on success or -1 for error
 *
 * This function adds an open file descriptor to the pool of event sources (up to MAX_EVENT_FDS)
 * and returns the fd on success or -1 on error, duplicate, or pool full. */
int event_add_fd(int fd);

/** \brief removes and fd from fd pool
 *  \param fd fd to remove
 *  \return fd on removal or -1 otherwise
 *
 * This function searches the pool and sets fd to -1 if found. None of the other functions
 * go out of their way to avoid reading from fd == -1, so removing fds is as yet untested.
 * Also returns fd if removed, so the caller can close it */
int event_remove_fd(int fd);

/** \brief adds event source by path
 *  \param path path to add
 *  \return fd on success, -1 otherwise
 *
 * This function opens path, and comares the fd using fstat to check for duplicates if the
 * fd isn't a duplicate of an existing fd in the pool then the fd is added to the pool.
 * event_add_fd does not check for duplicates so it is still possible to open the same source
 * twice and add it to the pool, if you would like to recieve some events twice for some reason. */
int event_add_source(const char * path);

/** \brief polls for events
 *  \param event pointer to copy the event into
 *  \return the source fd of the event, or 0
 *
 * This function reads the next event from any open source into the pointer event.
 * It returns 0 if no events are pending or the event's source fd otherwise */
int event_poll(struct input_event * event);

/** \brief gets name of event source
 *  \param fd ignored
 *  \note unstable api
 *
 *  This just runs the ioctl for getting the name of the input device. It will be reworked
 * in the future to search for fd in the pool and return a string. Right now it just prints out
 * the name of all open sources to stdout. For debugging purposes only do not use.*/
void event_get_name(int fd);

/** \brief gets phys of event source
 *  \param fd ignored
 *  \note unstable api
 *
 *  This runs the ioctl to get the phys of all fds in the pool. API unstable for debugging puposes
 * only */
void event_get_phys(int fd);

//debug functions
/** \brief prints fds in pool to stdout
 *
 * print out all of the open file descriptors. debugging only do not use. */
int event_debug_dump_fds();

#endif //H_EVENT
