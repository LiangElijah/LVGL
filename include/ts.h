#ifndef __TS_H__
#define __TS_H__

#include <stdio.h>
#include <stdlib.h>     //exit
#include <stdint.h>     //uintx_t
#include <sys/types.h>  //open
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>     //close
#include <strings.h>    //perror
#include <linux/input.h>

/*
* Input system read mode.
* 0     query
* 1     sleep
* 2     poll
* 3     async
*/
#define INPUT_READ_MODE 2

/* Sleep mode Do nothing */
#if (INPUT_READ_MODE == 2)
#include <poll.h>

/*
*  SIGNAL defines in "include\uapi\asm-generic\signal.h"
*/
#elif (INPUT_READ_MODE == 3)
#include <signal.h>
#endif

typedef struct focus_t
{
    unsigned int x;
    unsigned int y;
    unsigned char value;
}focus_t;

int ts_init(void);
void ts_deinit(void);

#if (INPUT_READ_MODE == 0 || INPUT_READ_MODE == 1)
void ts_default_handler(void *task);
#elif (INPUT_READ_MODE == 2)
void ts_poll_handler(void *task);
#elif (INPUT_READ_MODE == 3)
void ts_sig_handler(int signal);
#endif
void ts_probe_event(void);

void ts_get_focus(struct focus_t *f);

#endif  /* __TS_H__ */