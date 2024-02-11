#include <stdio.h>
#include "client.h"
int main(int argc, char** argv)
{
    printf("Hello, from udpclient!\n");
    // check if user enters serverIp or not?
    if (argc != 4) {
        printf("usage: udpcli <IPaddress> <Port Number> <CLIENT NAME>");
        exit(1);
    }

    // setup client and pass server IP
    setup_udp_client(argv[1], argv[2], argv[3]);

}
