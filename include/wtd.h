#ifndef __WTD_H__
#define __WTD_H__

#include <stdio.h>
#include <stdlib.h>     //exit
#include <stdint.h>     //uintx_t
#include <sys/types.h>  //open
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>     //close
#include <sys/ioctl.h>  //ioctl
#include <strings.h>    //perror
#include <linux/watchdog.h>

typedef enum WTD_STU
{
    WTD_ENABLE,
    WTD_DISABLE
}WTD_STU;

int wtd_init(void);
void wtd_deinit(void);
void wtd_get_info(void);
int wtd_set_time(int time);
void wtd_get_time(int *time);
int wtd_set_status(WTD_STU status);
void wtd_get_status(WTD_STU *status);
void wtd_keepalive(void);

#endif  /* __WTD_H__ */