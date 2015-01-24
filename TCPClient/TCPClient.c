/*
 * TCPClient.c
 * Systems and Networks II
 * Project 1
 * Christopher Schneider & Brett Rowberry
 *
 * This file implements TCPClient functions.
 * You may also implement any auxiliary functions you deem necessary.
 */

//from textbook p.115
#include <stdio.h>      /* for printf() and fprintf()*/
#include <stdlib.h>     /* for atoi() and exit() */
#include <sys/types.h>  /* for Socket data types */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <netinet/in.h> /* for IP Socket data types() */
#include <netdb.h>      /* */
#include <errno.h>      /* */
#include <signal.h>     /* */
#include <unistd.h>     /* for close() */
#include <string.h>     /* for memset() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <sys/wait.h>   /* */
#include "TCPClient.h"

const int connPort = 40000;

int main()
{
    struct sockaddr_in server;
    createSocket("cs-ssh3.cs.uwf.edu", 49227, &server);
    return 0;
}

/*
 * Creates a streaming socket and connects to a server.
 *
 * serverName - the ip address or hostname of the server given as a string
 * port       - the port number of the server
 * dest       - the server's address information; the structure should be created with information
 *              on the server (like port, address, and family) in this function call
 *
 * return value - the socket identifier or a negative number indicating the error if a connection could not be established
 */
int createSocket(char * serverName, int port, struct sockaddr_in * dest)
{
    int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //domain: AF_INET (for IPv4) | type: SOCK_STREAM | protocol: default
    struct hostent *hostptr = gethostbyname(serverName);

    if(socketFD < 0)
    {
        printf("Socket creation failed, socket");
        exit(1);
    }

    if(hostptr == NULL)
    {
        perror("gethostbyname() failed, exit\n"); //TODO implement retry every N seconds
        exit(1);
    }

    //testing
    printf("host %s", hostptr->h_name);

    struct in_addr ipAddress;
    int i=0;

    printf("\nIP: " );
    while(hostptr->h_addr_list[i] != 0)
    {
        ipAddress.s_addr = *(u_long*)hostptr->h_addr_list[i++];
        printf("%s\n", inet_ntoa(ipAddress));
    }

    memset((void*)dest, 0, sizeof(struct sockaddr_in));    /* zero the struct */
    dest->sin_family = AF_INET;
    memcpy( (void *)&dest->sin_addr, (void *)hostptr->h_addr, hostptr->h_length);
    dest->sin_port = htons( (u_short)port );        /* set destination port number */
    printf("port: %d\n", htons(dest->sin_port));

    if( connect( socketFD, (struct sockaddr *) dest, sizeof(struct sockaddr_in)) < 0)
        printf("Failed to connect");

    return 0;
}

/*
* Sends a request for service to the server. This is an asynchronous call to the server,
* so do not wait for a reply in this function.
*
* sock    - the socket identifier
* request - the request to be sent encoded as a string
* dest    - the server's address information
*
* return   - 0, if no error; otherwise, a negative number indicating the error
*/
int sendRequest(int sock, char * request, struct sockaddr_in * dest)
{
    write(sock, (struct sockaddr *)&dest, sizeof(dest));
//    write(sock, buf, strlen(buf) + 1);
    fgets( request, 256, stdin );
//    int sendlen = strlen( request ) - 1;
//    int bytes = sendto(sock, request, (size_t)dest->sin_len, 0,
//            (struct sockaddr *)&dest, (socklen_t)sizeof(dest) );
//    bind(sock, (struct sockaddr *)dest, dest->sin_len);
    return 0;
}

/*
* Receives the server's response formatted as an XML text string.
*
* sock     - the socket identifier
* response - the server's response as an XML formatted string to be filled in by this function;
*            memory is allocated for storing response
*
* return   - 0, if no error; otherwise, a negative number indicating the error
*/
int receiveResponse(int sock, char * response)
{
    char buffer[256]; /* +1 so we can add null terminator */
//    int len;

    bzero(buffer, 256); //instead of memset
    read(sock, buffer, 255);

//    connect(sock, (struct sockaddr *)&dest, sizeof(struct sockaddr));

//    len = recv(mysocket, buffer, 255, 0);

//    /* We have to null terminate the received data ourselves */
//    buffer[len] = '\0';
    return 0;
}

/*
* Prints the response to the screen in a formatted way.
*
* response - the server's response as an XML formatted string
*
*/
void printResponse(char* response)
{
    printf("%s", response);
}

/*
* Closes the specified socket
*
* sock - the ID of the socket to be closed
*
* return - 0, if no error; otherwise, a negative number indicating the error
*/
int closeSocket(int sock)
{
    int socketCloseError = close(sock);
    if(socketCloseError < 0) perror("Error closing socket.");
    return socketCloseError;
    //return EXIT_SUCCESS
}