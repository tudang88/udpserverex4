#ifndef UTILS_H_
#define UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// logger with time stamp prefix
void TIME_LOGGER(const char* format, ...);
// get current time stamp for output log
int get_current_time(char* buffer, size_t buffer_size);

#endif

