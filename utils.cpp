//
// Created by lmshao on 2021/10/29.
//

#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

char *getCurrentTime() {
    static char buff[24];
    struct timeval tv {};
    gettimeofday(&tv, nullptr);
    memset(buff, 0, 24);
    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S.", localtime(&tv.tv_sec));
    sprintf(buff + strlen(buff), "%03ld", tv.tv_usec / 1000);
    return buff;
}