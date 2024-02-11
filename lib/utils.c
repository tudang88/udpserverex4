#include "util.h"

/**
 * get current time stamp and store it in the provided buffer
 * return o on success, -1 on failure
*/
int get_current_time(char* buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size < 20) {
        // input condition failed
        return -1;
    }
    
    // Get timestamp and format it directly into the result
    time_t current_time = time(NULL);
    if (current_time == -1) {
        // Handle error
        return -1;
    }

    struct tm *time_info = localtime(&current_time);
    if (time_info == NULL) {
        // Handle error
        return -1;
    }

    if (strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_info) == 0) {
        // Handle error
        return -1;
    }

    return 0; // Success
}
