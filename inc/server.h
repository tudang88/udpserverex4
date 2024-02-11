#ifndef SERVER_H_
#define SERVER_H_
#include "unp.h"
#include "message.h"
#define TRUE 1
#define FALSE 0
#define MAX_CLIENTS 10
#define MAX_SOCKETS 2
#define MAX_NAME_SIZE 40


// server will maintain 
// a list of connected clients
typedef struct client_info {
    char name[MAX_NAME_SIZE];
    int connected_sockfd;
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
} client_info_t;

// server will maintain
// a list of opened server's
// address and port 
typedef struct server_t {
    u_int32_t address;
    u_int16_t port;
} server_info_t;

/**
 * server functions
*/
void remove_client(client_info_t *cli_list, const char *cli_name);
int register_client(client_info_t *cli_list, const char *cli_name);
void setup_server_socket(int sockfd, int port);
void setup_udp_server();
int get_max_fd(int *fd_list);
void transfer_service(int* sockfd_list);
void receive_from_client(int sockfd);
int get_target_cli_index(client_info_t *cli_list, const char *cli_name);
#endif // DEBUG