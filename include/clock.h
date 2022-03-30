#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdio.h>
#include <stdint.h>     //uintx_t
#include <errno.h>
//select
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>   //gettimeofday
#include <sys/types.h>
#include <unistd.h>
#include <time.h>       //clock_gettime

#if 0
#define GETTIMEOFDAY
#else
#define CLOCK_GETTIME
#endif

void select_sleep_s(uint32_t seconds);
void select_sleep_ms(uint32_t milliseconds);
void select_sleep_us(uint32_t microseconds);

void custom_tick_set(void);
uint64_t custom_tick_s_get(void);
uint64_t custom_tick_ms_get(void);
uint64_t custom_tick_us_get(void);
#if defined(CLOCK_GETTIME)
uint64_t custom_tick_ns_get(void);
#endif

#endif /* __CLOCK_H__ */