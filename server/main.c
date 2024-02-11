#include <stdio.h>
#include "server.h"
int main(int arg, char** argv)
{
    printf("Hello, from udpserver!\n");
    // setup server
    setup_udp_server();
}
