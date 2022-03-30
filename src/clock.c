#include "clock.h"

uint64_t start_s = 0, start_ms = 0, start_us = 0;
#if defined(CLOCK_GETTIME)
uint64_t start_ns = 0;
#endif

void select_sleep_s(uint32_t seconds)
{
    int err;
    struct timeval tv;

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    do{
        select(0, NULL, NULL, NULL, &tv);
    }while(err < 0 && errno == EINTR);
}

void select_sleep_ms(uint32_t milliseconds)
{
    int err;
    struct timeval tv;

    tv.tv_sec = milliseconds/1000;
    tv.tv_usec = ((milliseconds%1000)*1000);

    do{
        select(0, NULL, NULL, NULL, &tv);
    }while(err < 0 && errno == EINTR);
}

void select_sleep_us(uint32_t microseconds)
{
    int err;
    struct timeval tv;

    tv.tv_sec = microseconds/1000000;
    tv.tv_usec = microseconds%1000000;

    do{
        select(0, NULL, NULL, NULL, &tv);
    }while(err < 0 && errno == EINTR);
}

#if defined(GETTIMEOFDAY)
void custom_tick_set(void)
{
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    start_s = tv_start.tv_sec;
    start_ms = tv_start.tv_sec*1000 + tv_start.tv_usec/1000;
    start_us = tv_start.tv_sec*1000000 + tv_start.tv_usec;
}

uint64_t custom_tick_s_get(void)
{
    struct timeval tv_end;
    uint64_t end_s;
    gettimeofday(&tv_end, NULL);
    end_s = tv_end.tv_sec;

    uint64_t time_s = end_s - start_s;
    return time_s;
}

uint64_t custom_tick_ms_get(void)
{
    struct timeval tv_end;
    uint64_t end_ms;
    gettimeofday(&tv_end, NULL);
    end_ms = tv_end.tv_sec*1000 + tv_end.tv_usec/1000;

    uint64_t time_ms = end_ms - start_ms;
    return time_ms;
}

uint64_t custom_tick_us_get(void)
{
    struct timeval tv_end;
    uint64_t end_us;
    gettimeofday(&tv_end, NULL);
    end_us = tv_end.tv_sec*1000000 + tv_end.tv_usec;

    uint64_t time_us = end_us - start_us;
    return time_us;
}
#elif defined(CLOCK_GETTIME)
void custom_tick_set(void)
{
    struct timespec tv_start;
    clock_gettime(CLOCK_MONOTONIC, &tv_start);
    start_s = tv_start.tv_sec;
    start_ms = tv_start.tv_sec*1000 + tv_start.tv_nsec/1000000;
    start_us = tv_start.tv_sec*1000000 + tv_start.tv_nsec/1000;
    start_ns = tv_start.tv_sec*1000000000 + tv_start.tv_nsec;
}

uint64_t custom_tick_s_get(void)
{
    struct timespec tv_end;
    uint64_t end_s;
    clock_gettime(CLOCK_MONOTONIC, &tv_end);
    end_s = tv_end.tv_sec;

    uint64_t time_s = end_s - start_s;
    return time_s;
}

uint64_t custom_tick_ms_get(void)
{
    struct timespec tv_end;
    uint64_t end_ms;
    clock_gettime(CLOCK_MONOTONIC, &tv_end);
    end_ms = tv_end.tv_sec*1000 + tv_end.tv_nsec/1000000;

    uint64_t time_ms = end_ms - start_ms;
    return time_ms;
}

uint64_t custom_tick_us_get(void)
{
    struct timespec tv_end;
    uint64_t end_us;
    clock_gettime(CLOCK_MONOTONIC, &tv_end);
    end_us = tv_end.tv_sec*1000000 + tv_end.tv_nsec/1000;

    uint64_t time_us = end_us - start_us;
    return time_us;
}

uint64_t custom_tick_ns_get(void)
{
    struct timespec tv_end;
    uint64_t end_ns;
    clock_gettime(CLOCK_MONOTONIC, &tv_end);
    end_ns = tv_end.tv_sec*1000000000 + tv_end.tv_nsec;

    uint64_t time_ns = end_ns - start_ns;
    return time_ns;
}
#endif