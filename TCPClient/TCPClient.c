/*
 * TCPClient.c
 * Systems and Networks II
 * Project 1
 * Christopher Schneider & Brett Rowberry
 *
 * This file implements TCPClient functions.
 * You may also implement any auxiliary functions you deem necessary.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

//#include <tclDecls.h>
#include <tcl.h>
//#include <tclPlatDecls.h>
//#include "TCPClient.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    struct sockaddr_in myaddr;
    int sockfd;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(3490);
    inet_aton("www.uwf.edu", &myaddr.sin_addr.s_addr);
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    bind(sockfd, (struct sockaddr*)myaddr, sizeof(myaddr));
    if(sockfd < 0) perror("Error creating socket");
    return sockfd;
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
}