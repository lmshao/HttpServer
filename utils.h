//
// Created by lmshao on 2021/10/29.
//

#ifndef _UTILS_H
#define _UTILS_H

#define LOG(fmt...)  \
    do {             \
        printf(fmt); \
    } while (0)

#define LOGD(fmt...)                                                             \
    do {                                                                         \
        printf("\033[1;32m%s - %s:%d - ", getCurrentTime(), __FILE__, __LINE__); \
        printf(fmt);                                                             \
        printf("\n\033[0m\n");                                                   \
    } while (0)

#define LOGW(fmt...)                                                             \
    do {                                                                         \
        printf("\033[1;33m%s - %s:%d - ", getCurrentTime(), __FILE__, __LINE__); \
        printf(fmt);                                                             \
        printf("\n\033[0m\n");                                                   \
    } while (0)

#define LOGE(fmt...)                                                           \
    do {                                                                       \
        printf("\033[1;31m%s [%s:%d]:", getCurrentTime(), __FILE__, __LINE__); \
        printf(fmt);                                                           \
        printf("\n\033[0m\n");                                                 \
    } while (0)

char *getCurrentTime();

#endif  //_UTILS_H
