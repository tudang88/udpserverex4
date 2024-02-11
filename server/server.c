#include "server.h"
#include "utils.h"
const char RESPONSE_OK[] = "CLIENT REGISTERED SUCCESSFUL";
const char RESPONSE_ERR[] = "ERROR INVALID CLIENT NAME";
const char RESPONSE_ERR_DISCONNECTED[] = "ERROR RECEIVER CLIENT DISCONNECTED";

/**
 * Global variables
*/
client_info_t connected_clients[MAX_CLIENTS] = {0};

/**
 * setup server socket
*/
void setup_server_socket(int sockfd, int port) 
{
    struct sockaddr_in server_addr;
    const int on = 1;

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Set socket option to reuseable
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Bind socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        TIME_LOGGER("Socket binding error\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * set up udp server for echo function
*/
void setup_udp_server()
{
    int socket_list[MAX_SOCKETS];
    struct sockaddr_in server_addrs[MAX_SOCKETS];
    int server_ports[MAX_SOCKETS] = {SERV_PORT1, SERV_PORT2};
    struct sockaddr_in cliaddr;
    const int on = 1;

    // Create server sockets
    for (int i = 0; i < MAX_SOCKETS; i++) {
        TIME_LOGGER("Create server socket\n");
        // Create socket
        if ((socket_list[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket");
            TIME_LOGGER("Socket openning error\n");
            exit(EXIT_FAILURE);
        }

        // Setup server socket
        setup_server_socket(socket_list[i], (i == 0) ? SERV_PORT1 : SERV_PORT2);
    }

    // call transfer service
    transfer_service(socket_list);
}

/**
 * calculate the max FD
*/
int get_max_fd(int *fd_list)
{
    // error handling
    if (fd_list == NULL) {
        return -1;
    }

    // get max FD
    int max = *fd_list;
    for (int i=0; i<MAX_SOCKETS; i++) {
        if (*(fd_list + i) > max) {
            max = *(fd_list + i);
        }
    }

    return max;
}

/**
 * re-initialize read fds
*/
void re_init_readfds(fd_set *fd_set_prt, const int *sockfd_list)
{
    FD_ZERO(fd_set_prt);
    for(int i=0; i<MAX_SOCKETS; i++) {
        FD_SET(sockfd_list[i], fd_set_prt);
    }    
}

/**
 * main service of server
 * lauching an iterative server
 * transfer package from client to client
*/
void transfer_service(int* sockfd_list)
{
    TIME_LOGGER("transfer_service -> START\n");

    int nready;
    client_info_t connected_clients[MAX_CLIENTS];
    fd_set rset;

    // monitor and handle incomming package
    for ( ; ; ) {
        TIME_LOGGER("transfer_service -> select()\n");
        re_init_readfds(&rset, sockfd_list);
        nready = select(get_max_fd(sockfd_list) + 1, &rset, NULL, NULL, NULL);
        // test all sockets
        for (int i=0; i<MAX_SOCKETS; i++) {
            // retrieve sockfd from socket list
            int sockfd = sockfd_list[i];                
            TIME_LOGGER("socketfd:%d READY\n", sockfd);

            // examine the socket ready for i/o or not yet
            if (FD_ISSET(sockfd, &rset)) {
                // socket i ready for i/o
                TIME_LOGGER("transfer_service -> socketfd READY\n");
                receive_from_client(sockfd);                
                // all ready sockets has been serviced
                if (--nready <= 0) break;
            }
        }
    }
    
    TIME_LOGGER("transfer_service -> END\n");

}

/**
 * remove one client from connected list
 * re-order the list
*/
void remove_client(client_info_t *cli_list, const char *cli_name)
{
    // Find the client to remove
    int del_idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(cli_name, cli_list[i].name) == 0) {
            del_idx = i;
            break;
        }
    }

    // If client found, remove it and shift items from the right
    if (del_idx != -1) {
        // Shift items from the right to fill the gap
        for (int i = del_idx + 1; i < MAX_CLIENTS; i++) {
            cli_list[i - 1] = cli_list[i];
            // clear the obsoleted item
            memset(&cli_list[i], 0, sizeof(client_info_t));
        }
    }
}

/**
 * valid new client name
*/
int register_client(client_info_t *cli_list, const char *cli_name)
{
    int res = -1;
    // check if name is already used or not
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // found available entry 
        if (cli_list[i].cli_len == 0) {
            res = i;
            break;
        }
        // Check if name is already used
        if (strcmp(cli_name, cli_list[i].name) == 0) {
            return -1;
        }
    }

    // return avaibale index for valid client
    return res;
}

/**
 * find client index
*/
int get_target_cli_index(client_info_t *cli_list, const char *cli_name)
{
    TIME_LOGGER("find index of target name:%s len: %d\n", cli_name, strlen(cli_name));
    int idx = -1;
    // check if name is already used or not
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // Check if name is already used
        if (strncmp(cli_name, cli_list[i].name, strlen(cli_name) -1) == 0) {
            TIME_LOGGER("check connected cli_name:%s len:%d\n", cli_list[i].name, strlen(cli_list[i].name));
            idx = i;
            break;
        }
    }

    return idx;
}

/**
 * receive datagram when
 * socketfd ready
*/
void receive_from_client(int sockfd)
{
    TIME_LOGGER("receive_from_client -> START\n");

    // receive from client
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(struct sockaddr_in);
    char buffer[MAXLINE];
    message_t message;
    int n = recvfrom(sockfd, buffer, MAXLINE, 0, (SA *)&cliaddr, &clilen);


    if (n<0) {
       TIME_LOGGER("ERROR on recvfrom\n");
    } else {
        // check the message type
        TIME_LOGGER("receive_from_client -> receive message\n");
        if (deserialize_message(buffer, &message) !=0) {
            TIME_LOGGER("ERROR on deserialize_message()\n");
            return;
        }

        // confirm message type
        switch (message.type)
        {
        case TYPE_CMD:
            {
                TIME_LOGGER("Receive TYPE_CMD from %s\n", message.from);
                // confirm client name valid or not
                int cli_idx = register_client(connected_clients, message.from);
                if (cli_idx >= 0) {
                    client_info_t cli;
                    strncpy(cli.name, message.from, MAX_NAME_SIZE - 1); // ensure NULL terminate
                    cli.name[MAX_NAME_SIZE -1] = '\0';
                    cli.cli_len = clilen;
                    cli.cli_addr = cliaddr;
                    cli.connected_sockfd = sockfd;
                    connected_clients[cli_idx] = cli;
                    // SEND SUCCESS to client
                    char sendline[MAXLINE];
                    message_t msg = {"SERVER", cli.name, TYPE_RES, RESPONSE_OK};
                    // only for initializing connection
                    if (serialize_message(&msg, sendline, sizeof(sendline)) != 0) {
                        TIME_LOGGER("Error on serialize_message()!!\n");
                    }

                    sendto(sockfd, (void*) sendline, sizeof(sendline), 0, (SA *)&cliaddr, clilen);
                } else {
                    TIME_LOGGER("Error client_name has been used!! %s\n", message.from);
                    // SEND Error to client
                    char sendline[MAXLINE];
                    message_t msg = {"SERVER", message.from, TYPE_RES, RESPONSE_ERR};
                    // only for initializing connection
                    if (serialize_message(&msg, sendline, sizeof(sendline)) != 0) {
                        TIME_LOGGER("Error on serialize_message()!!\n");
                    }

                    sendto(sockfd, (void*) sendline, sizeof(sendline), 0, (SA *)&cliaddr, clilen);
                    
                }

            }
            break;
        case TYPE_MSG:
            {
                TIME_LOGGER("Receive TYPE_MSG from %s\n", message.from);
                // find target client
                int to_cli_idx = get_target_cli_index(connected_clients, message.to);
                if (to_cli_idx == -1) {
                    TIME_LOGGER("ERROR Target client NOT found! idx:%d\n", to_cli_idx);
                    // Target cli disconnected
                    char sendline[MAXLINE];
                    message_t msg = {"SERVER", message.from, TYPE_MSG, RESPONSE_ERR_DISCONNECTED};
                    // only for initializing connection
                    if (serialize_message(&msg, sendline, sizeof(sendline)) != 0) {
                        TIME_LOGGER("Error on serialize_message()!!\n");
                    }

                    sendto(sockfd, (void*) sendline, sizeof(sendline), 0, (SA *)&cliaddr, clilen);
                } else {
                    TIME_LOGGER("Target client found! idx:%d\n", to_cli_idx);
                    char sendline[MAXLINE];
                    message_t msg = {message.from, message.to, TYPE_MSG, message.content};
                    // only for initializing connection
                    if (serialize_message(&msg, sendline, sizeof(sendline)) != 0) {
                        TIME_LOGGER("Error on serialize_message()!!\n");
                    }
                    // transfer message to receiver client
                    sendto(connected_clients[to_cli_idx].connected_sockfd, (void*) sendline, 
                        sizeof(sendline), 0, 
                            (SA *)&connected_clients[to_cli_idx].cli_addr, 
                                connected_clients[to_cli_idx].cli_len);
                }

            }
            break;
        case TYPE_END:
            {
                // unregister a client
                TIME_LOGGER("Receive TYPE_END from %s\n", message.from);
                remove_client(connected_clients, message.from);

            }
            break;
        default:
            {
                TIME_LOGGER("Receive UNSUPPORTED message from %s\n", message.from);
            }
            break;
        }

    }
    // free memory allocated by strdup in deserialize_message()
    free((void *)message.from);
    free((void *)message.to);
    free((void *)message.content);
    TIME_LOGGER("receive_from_client -> END\n");
}