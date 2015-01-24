/*
 * TCPmain.c
 * Systems and Networks II
 * Project 1
 * Christopher Schneider & Brett Rowberry
 *
 * This is a tester file for TCPClient.c
 */

#include "TCPClient.h"
#include "headerFiles.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("hello\n");

    int socketFD;
    int numBytesInRecvCall;
    char *serverName = "www.uwf.edu";
    int serverPort = 80;
    char *request = "";
    int len;
    char buffer[256 + 1];
    char *ptr = buffer;
    struct sockaddr_in serverAddr;

    socketFD = createSocket(serverName, serverPort, &serverAddr);
    sendRequest(socketFD, request, &serverAddr);

    printf("goodbye\n");

    return 0;
}