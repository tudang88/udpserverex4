#ifndef CLIENT_H_
#define CLIENT_H_
#include "unp.h"
#define TRUE 1
#define FALSE 0
#define MAX_CLIENTS 10
#define MAX_SOCKETS 2
#define MAX_NAME_SIZE 40

// server will maintain 
// a list of connected clients
typedef struct client_info {
    const char *name;
    int connected_sockfd;
    struct sockaddr_in *cli_addr;
    socklen_t cli_len;
} client_info_t;

// server information 
// for receiving thread argument
typedef struct server_t {
    int sockfd;
    struct sockaddr_in *pserveraddr;
    socklen_t serverlen;
} server_info_t;

void setup_udp_client(char *server_address, char* port, char *client_name);
void cli_service(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servelen);
void start_receiving_thread(int sockfd);
void receive_print_message(int sockfd);
void send_message(int sockfd, const SA *pservaddr, socklen_t servelen);
#endif // DEBUG