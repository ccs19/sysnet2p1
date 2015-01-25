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

const int BUFFERSIZE = 256;

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

    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    int socketFD;
    struct hostent *hostptr = gethostbyname(serverName);
    struct in_addr ipAddress;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if( (socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) < 0)
    {
        printf("Socket creation failed, socket");
        return -1;
    }
    if( (hostptr = gethostbyname(serverName) ) == NULL)
    {
        perror("gethostbyname() failed, exit\n");
        return -1;
    }

    //TODO remove debug code and clean this up
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


    if( connect( socketFD, (struct sockaddr *) dest, sizeof(struct sockaddr_in)) < 0) {
        printf("Failed to connect");
        return -1;
    }

    return socketFD;
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

    if( (send(sock, request, strlen(request), 0) ) < 0) {
        printf("Send failed\n");
        return -1;
    }
    else {
        printf("Sending %s\n", request);
        return 0;
    }
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
    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);

    int readReturn = read(sock,buffer,BUFFERSIZE);
    if( readReturn < 0 || readReturn > BUFFERSIZE )
    {
        printf("Read failed\n");
        return readReturn;
    }
    else
    {
        bzero(response, BUFFERSIZE);
        strcat(response, buffer); //Copy buffer into response
        return 0;
    }

}

/*
* Prints the response to the screen in a formatted way.
*
* response - the server's response as an XML formatted string
*
*/
void printResponse(char* response)
{
    printf("printResponse %s\n", response);
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
    int closeSocketResponse = close(sock);
    if(closeSocketResponse < 0)
    {
       printf("Failed to close socket\n");
        return closeSocketResponse;
    }
    else
        return 0;
}