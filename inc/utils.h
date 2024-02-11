#ifndef UTILS_H_
#define UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Define a macro for logging
*/
#define LOGGER(...) fprintf(stdout, ##__VA_ARGS__)
#define TIME_LOGGER(format, ...) do{\
    char time_stamp[20];\
    get_current_time(time_stamp, 20);\
    LOGGER("%s " format, time_stamp, ##__VA_ARGS__);\
}while(0)


// get current time stamp for output log
int get_current_time(char* buffer, size_t buffer_size);

#endif

