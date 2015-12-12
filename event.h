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
#include <string.h>

#define MAX_EVENT_FDS 256
#define EVENT_STRING_BUF_SIZE 256

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
 * It returns 0 if no events are pending or the event's source fd otherwise. The fd pool
 * is iterated and the first fd ready to read is returned, this means the last source
 * added is always read first. It's possible that a heavy event stream might result
 * in dropped events when a lot of sources are present. */
int event_poll(struct input_event * event);

/** \brief compares 2 events
 *  \param a an event pointer
 *  \param b another event pointer
 *  \return 1 if events match else 0
 *
 * This function compares two events and determines if they match or not.
 * The timestamp is ignored, use event_is_newer to check timestamps. */
int event_compare(struct input_event *a, struct input_event *b);

/** \brief compare the timestamps of 2 events
 *  \param a event to test
 *  \param b event tested agains
 *  \return 1 if a > b, 0 if a == b and -1 if a < b
 *
 * Compare the timestamps of two events. */
int event_is_newer(struct input_event *a, struct input_event *b);

/** \brief copy an input event
 *  \param src source event
 *  \param dest event
 *  \return pointer to dest
 *  \note this is supposed to be defined inline but i'm not sure how that works
 * 		worst case is you get a real function instead of inlined code
 * 
 * wraps memcpy of src to dest. */
struct input_event* event_copy(struct input_event *src, struct input_event *dest);

/** \brief gets input id of source
 *  \param fd fd to get input id of
 *  \return pointer to struct input_id
 *  \note all get_info functions share a buffer, caller must copy value before
 * 		calling another event_get_foo.
 * 
 * This function populates a statically allocated buffer with information from ioctl. */
struct input_id * event_get_id(int fd);

/** \brief gets name of event source
 *  \param fd fd to get name of
 *  \return char * name
 *  \note all get_info functions share a buffer, caller must copy value before
 * 		calling another event_get_foo.
 * 
 * Populates static buffer with name of input device via ioctls.*/
char * event_get_name(int fd);

/** \brief gets phys of event source
 *  \param fd fd to get phys of
 *  \return char * phys
 *  \note all get_info functions share a buffer, caller must copy value before
 * 		calling another event_get_foo.
 *
 * Populates static buffer with info from ioctl.*/
char * event_get_phys(int fd);

/** \brief get input device info
 *  \param fd fd of input device
 *  \return char * to string
 *  \note all get_info functions share a buffer, caller must copy value before
 * 		calling another event_get_foo.
 * 
 * This populates a static buffer with info from ioctl. */
char * event_get_info(int fd);

//debug functions
/** \brief prints fds in pool to stdout
 *
 * print out all of the open file descriptors. debugging only do not use. */
int event_debug_dump_fds();

#endif //H_EVENT
