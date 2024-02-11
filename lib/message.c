#include "message.h"

/**
 * Implementation of message related functions
*/

// Function to serialize a message structure into a byte stream
int serialize_message(const message_t *message, char *buffer, size_t buffer_size) {
    if (snprintf(buffer, buffer_size, "%s|%s|%d|%s", message->from, message->to, message->type, message->content) >= buffer_size) {
        // Buffer too small
        return -1;
    }
    return 0;
}

// Function to deserialize a byte stream into a message structure
int deserialize_message(const char *buffer, message_t *message) {
    // Tokenize the buffer using '|'
    char *token = strtok(buffer, "|");
    if (token == NULL) return -1;
    message->from = strdup(token);
    // printf("Message From: %s \n", message->from);
    token = strtok(NULL, "|");
    if (token == NULL) return -1;
    message->to = strdup(token);
    // printf("Message To: %s \n", message->to);

    token = strtok(NULL, "|");
    if (token == NULL) return -1;
    message->type = atoi(token);
    // printf("Message Type: %d \n", message->type);

    token = strtok(NULL, "|");
    if (token == NULL) return -1;
    message->content = strdup(token);
    // printf("Message Content: %s \n", message->content);

    return 0;
}