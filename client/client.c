#include "client.h"
#include "message.h"
#include "utils.h"

const char RESPONSE_OK[] = "CLIENT REGISTERED SUCCESSFUL";
const char RESPONSE_ERR[] = "ERROR INVALID CLIENT NAME";
char CLI_NAME[MAX_NAME_SIZE];
/**
 * set up a client colect input from standard input
 * then send to server
 * and print the response from server
*/
void setup_udp_client(char *server_address, char* port, char *cliname)
{
    int sockfd;
    struct sockaddr_in serveraddr;

    int server_port = atoi(port);

    /**
     * check ip address
    */
    if (strlen(server_address) == 0 || strcmp(server_address, "") == 0) {
        TIME_LOGGER("Server IP incorrect: %s\n", server_address);
        exit(1);
    }

    /**
     * check port number
    */
    if (server_port != SERV_PORT1 && server_port != SERV_PORT2) {
        TIME_LOGGER("PORT NUMBER INVALID:%d\n", server_port);
        TIME_LOGGER("Please set port number equals to 9877 or 9878\n");
        exit(1);
    }


    /**
     * check client name
    */
    if (strlen(cliname) == 0 || strcmp(cliname, "") == 0) {
        TIME_LOGGER("ERROR Client Name Empty\n");
        exit(1);
    }
    // Copy client name to CLI_NAME
    strncpy(CLI_NAME, cliname, MAX_NAME_SIZE - 1);
    CLI_NAME[MAX_NAME_SIZE - 1] = '\0'; // Ensure null-termination

    // config target server address
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(server_port);
    // convert IP format from string "aa.bb.cc.dd" to correct form
    inet_pton(AF_INET, server_address, &serveraddr.sin_addr);
    
    // Create socket for client
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // call service function
    cli_service(stdin, sockfd, (SA *) &serveraddr, sizeof(serveraddr));

    exit(0);
}


/**
 * set up connection with server
*/
void set_up_connection(int sockfd, const SA *pservaddr, socklen_t servelen)
{
    int n;
    char buffer[MAXLINE];
    message_t msg = {CLI_NAME, "SERVER", TYPE_CMD, "SETUP"};
    // reset buffer
    memset(buffer, 0, sizeof(buffer));
    // only for initializing connection
    if (serialize_message(&msg, buffer, sizeof(buffer)) != 0) {
        TIME_LOGGER("Setup connection ERROR!!\n");
        TIME_LOGGER("Error on deserialize_message()\n");
        exit(1);
    }
    /**
     * send setup request
    */
    TIME_LOGGER("Send Setup request\n");
    sendto(sockfd, (void*) buffer, sizeof(buffer), 0, pservaddr, servelen);

    /**
     * wait setup response
    */
    // reset buffer
    memset(buffer, 0, sizeof(buffer));
    memset(&msg, 0, sizeof(message_t));
    n = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
    if (deserialize_message(buffer, &msg) != 0) {
        TIME_LOGGER("Error on deserialize_message()\n");
    } else {
        switch (msg.type)
        {
        case TYPE_RES:
            {
                TIME_LOGGER("%s\n", msg.content);
                if (strcmp(msg.content, RESPONSE_OK) == 0) {
                    /**
                    * start receiving thread
                    */
                    start_receiving_thread(sockfd);
                    /**
                     * start get user input and send to 
                     * target client
                    */
                    send_message(sockfd, pservaddr, servelen);
                } else if (strcmp(msg.content, RESPONSE_ERR) == 0) {
                    /**
                     * error on setup connection
                    */
                    exit(1);
                }
            }
            break;
        default:
                TIME_LOGGER("SERVER RESPONSE NOT CORRECT TYPE:%d\n", msg.type);
                exit(1);
            break;
        }
        // free memories allocated by strdup in deserialize_message()
        free((void *)msg.from);
        free((void *)msg.to);
        free((void *)msg.content);
    }
    // clear receive buffer
    memset(buffer, 0, sizeof(buffer));
}
/**
 * get user input
*/
void send_message(int sockfd, const SA *pservaddr, socklen_t servelen)
{
    char line[MAXLINE];
    char to_client_name[MAX_NAME_SIZE];
    char content[MAXLINE];
    TIME_LOGGER("========**=====***=====**=========\n");
    for( ; ; ) {
        // get destination
        TIME_LOGGER("TO:");
        if (fgets(line, sizeof(line), stdin)) {
            strncpy(to_client_name, line, MAX_NAME_SIZE-1);
            to_client_name[MAX_NAME_SIZE-1] = '\0';
        }

        // get message
        TIME_LOGGER("MESSAGE:");
        if (fgets(line, sizeof(line), stdin)) {
            strcpy(content, line);
        }
        // ignore empty info
        if (strcmp(to_client_name, "") == 0 || strcmp(content, "") == 0)
            continue;
        /**
         * send message here
        */
       memset(line, 0, sizeof(line));
       message_t msg = {CLI_NAME, to_client_name, TYPE_MSG, content};
       if (serialize_message(&msg, line, sizeof(line)) != 0) {

       }

       /**
        * send datagram
       */
       sendto(sockfd, (void*) line, sizeof(line), 0, pservaddr, servelen);
    }
}

/**
 * start a thread for receiving only
*/
void start_receiving_thread(int sockfd)
{
    pthread_t id; // declare a threadId
    pthread_create(&id, NULL, (void *)&receive_print_message, sockfd);
}

/**
 * The function of receiving thread
*/
void receive_print_message(int sockfd)
{
    // TIME_LOGGER("receive thread sockfd:%d\n", sockfd);

    while(TRUE) 
    {
        // receive
        int n;
        char buffer[MAXLINE];
        message_t message;
        n = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
        if (deserialize_message(buffer, &message) != 0) {
            TIME_LOGGER("Error on deserialize_message()\n");
        } else {
            switch (message.type)
            {
            case TYPE_MSG:
                {
                    // show message
                    TIME_LOGGER("\nFrom: %s, To: %s, msg: %s\n", message.from, message.to, message.content);
                }
                break;
            default:
                TIME_LOGGER("SERVER RESPONSE NOT CORRECT TYPE:%d\n", message.type);
                break;
            }
            // free memories allocated by strdup in deserialize_message()
            free((void *)message.from);
            free((void *)message.to);
            free((void *)message.content);
        }

        // clear receive buffer
        memset(buffer, 0, sizeof(buffer));

    }
}

/**
 * client function will colect the standard input, then send to the server
*/
void cli_service(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servelen)
{
    /**
     * setup connection with server before chatting
    */ 
    set_up_connection(sockfd, pservaddr, servelen);

}