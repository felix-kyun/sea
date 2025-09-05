#include "utils.h"
#include <stdio.h>
#include <time.h>

void msleep(long msec)
{
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void padding(int count)
{
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}
