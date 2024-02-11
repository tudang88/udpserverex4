#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// message type
#define TYPE_CMD 11
#define TYPE_MSG 22
#define TYPE_RES 33
#define TYPE_END 44
// a message struct transferred between client to client
typedef struct message {
    const char *from; // name of sender
    const char *to; // name of receiver
    int type; // TYPE_MSG or TYPE_CMD
    char *content;
} message_t;
/**
 * helper functions
*/
int serialize_message(const message_t *msg, char *buffer, size_t buffer_size);
int deserialize_message(const char *buffer, message_t *msg);

#endif // MESSAGE_H