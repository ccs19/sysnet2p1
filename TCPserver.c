/*
 ============================================================================
 Name        : TCPserver.c
 Author      : Christopher Schneider, Brett Rowberry
 Description : A TCP server that makes a connection and receives strings from a client
 ============================================================================
 */



#include <stdio.h>
#include <stdlib.h> //For getloadavg()
#include <netdb.h> //For gethostbyname()
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> //for memset


#include "TCPserver.h"


//Globals
int ServerSocket = 0;
struct hostent *HostByName = NULL;
struct sockaddr_in ServerAddress;

//Constants
const int HostNameMaxSize = 256;
const int MAXLISTENERS = 5;
const int BUFFERSIZE = 256;
const int PORT_UNDEFINED = 99999;
const int MAX_PORT = 65535;




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   main

    Accepts a port number in argv[1] and opens a server on that port.

    @param            argv  -- The argument in argv[1] represents the port number
    @return           0

 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int main(int argc, const char* argv[])
{
    unsigned int port = PORT_UNDEFINED;

    if(argc > 2)
        printf("Usage:\n%s <optional:port to open>\n",argv[0]);
    else if(argc == 1) //If no port specified
        port = 0;
    else if(atoi(argv[1]) <= 0 || atoi(argv[1]) > MAX_PORT) //Checking valid port bounds
        printf("Invalid port number\n");
    else {
        if (port == PORT_UNDEFINED) port = atoi(argv[1]); //If port specified at command line
    }

    if(port != PORT_UNDEFINED)
    {
        OpenSocket(port); //Open socket with port in arg vector 1
        AcceptConnections();
    }
    return 0;
}





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   OpenSocket

    Opens the listening socket and resolves the host name.

    @param  port           -- The port number to bind the listen socket
    @return                -- void

 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void OpenSocket(int port)
{
    char hostname[HostNameMaxSize];

    if( ( ServerSocket =  socket(PF_INET, SOCK_STREAM, 0) ) <  0)  //If socket fails
        ExitOnError("Error creating socket");

    if(gethostname(hostname, sizeof(hostname)) < 0)                         //If getting hostname fails
        ExitOnError("Error acquiring hostname. Exiting");

    if( ( HostByName = gethostbyname(hostname) ) ==  NULL)                  //If gethostbyname fails print error message, exit
    {
        herror("GetHostByName failed. Error: ");
        exit(1);
    }
    InitAddressStruct(port);
    BindSocketAndListen();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   InitAddressStruct

    Initializes the ServerAddress structure with the IP, port, and protocol type.

    @param  port           --  Port number to open
    @return                --  void
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void InitAddressStruct(int port)
{
    memset((void*)&ServerAddress, '0', (size_t)sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    memcpy( (void *)&ServerAddress.sin_addr, (void *)HostByName->h_addr, HostByName->h_length);
    ServerAddress.sin_port = htons(port);
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   DisplayInfo

    Displays the connection info of the server.

    @return           --    void
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void DisplayInfo()
{
    int i = 0;
    struct in_addr ipAddress;
    struct sockaddr_in sockIn;
    int sockLen = sizeof(sockLen);
    getsockname(ServerSocket, (struct sockaddr*)&sockIn, (socklen_t*)&sockLen );
    printf("Host Name: %s\n",HostByName->h_name);
    printf("IP:        ");
    while(HostByName->h_addr_list[i] != 0)
    {
       ipAddress.s_addr = *(u_long*)HostByName->h_addr_list[i++];
        printf("%s\n", inet_ntoa(ipAddress));
    }
    printf("Port:      %d\n", htons(sockIn.sin_port));
    fflush(stdout);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   BindSocketAndListen

    Binds the server socket and listens on that socket

    @return           --    void


 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void BindSocketAndListen()
{
    if( ( bind( ServerSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)) )  < 0)
        ExitOnError("Failed to bind socket"); //If binding of socket fails
    if( (listen( ServerSocket, MAXLISTENERS)) < 0)
        ExitOnError("Failed to listen");
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   AcceptConnections

    Accepts the client connection and creates a new detached thread for the client.

    @return           --    void
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void AcceptConnections()
{
    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    struct sockaddr_in ClientAddress;
    unsigned int clientAddressSize = sizeof(ClientAddress);
    pthread_attr_t ThreadAttribute;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    DisplayInfo();
    pthread_attr_init(&ThreadAttribute);
    pthread_attr_setdetachstate(&ThreadAttribute, PTHREAD_CREATE_DETACHED);
    for(;;)
    {
        pthread_t DetachedThread;
        int *ClientSocket = malloc(sizeof(int));
        printf("Waiting for connection... ");
        fflush(stdout);
        if( (*ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddress, &clientAddressSize) ) < 0)
            ExitOnError("Error in accept()");
        pthread_create(&DetachedThread, &ThreadAttribute, (void*)HandleClientRequests, (void*)ClientSocket);

    }
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   ExitOnError

    Prints a message to stdout and exits

    @param  errorMessage           -- Error message to be printed
    @return                        -- void
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void ExitOnError(char* errorMessage)
{
    printf("%s\n", errorMessage);
    exit(1);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   HandleClientRequests

    Depending on the string received from the client, we either print that it failed or parse
    the message, then close the socket and free the thread.

    @param  ClientSocketPtr -- A pointer to the client socket typecasted to a void*
    @return           -- void
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void HandleClientRequests(void* ClientSocketPtr)
{

    //TODO: REMOVE DEBUG STATEMENTS
    printf("Client connected!\n");
    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    int ClientSocket = *(int*)ClientSocketPtr;
    char stringBuffer[BUFFERSIZE];
    bzero(stringBuffer, BUFFERSIZE);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    fflush(stdout);
    if( (read(ClientSocket, stringBuffer, sizeof(stringBuffer))) < 0) //If read message fails
    {
        printf("Failed to read message from client\n");
    }
    else //Else parse message and do something.
    {
        //stringBuffer[msgSize + 1] = '\0';
        printf("Received message: %s\n", stringBuffer);
        ParseClientMessage(stringBuffer, ClientSocket);
    }
    close(ClientSocket);
    free(ClientSocketPtr);
    pthread_exit(NULL);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION:   ParseClientMessage

    Handles the message for the client and sends a message back to the client

    @param  clientMessage        -- Pointer to message received by client
    @param  ClientSocket         -- The socket to the client
    @return                      -- void


 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void ParseClientMessage(char* clientMessage, int ClientSocket)
{
    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    int i = 0;
    char string[BUFFERSIZE]; //String to send back to client
    char token[BUFFERSIZE];  //Token to use for echo reply
    string[0] = '\0';
    const int NUMLOADAVG = 3; //Number of load averages queries
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /*~~~~~~~~~~~~~~~~~~~~~Load avg response~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(strcmp(clientMessage, "<loadavg/>") == 0)
    {
        double loadavg[NUMLOADAVG];
        getloadavg(loadavg, NUMLOADAVG); //Get load average and put in double array

        //Begin string format:
        strcat(string, "<replyLoadAvg>");
        for(i = 0; i < NUMLOADAVG; i++)
        {
            char tempAvg[BUFFERSIZE];
            sprintf(tempAvg, "%lf:", loadavg[i]);
            strcat(string, tempAvg);              //Append string to string we return to client
        }
        strcat(string, "</replyLoadAvg>\0"); //End of string
    }
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*~~~~~~~~~~~~~~~~~~~~~Echo response~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    else if ((XMLParser("<echo>", "</echo>", clientMessage, token, sizeof(token))) == 1)
    {
       //Set return echo string
       strcat(string, "<reply>");
       strcat(string, token);
       strcat(string, "</reply>\0");
    }
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    else //Else we have an invalid format
        strcat(string, "<error>unknown format</error>\0");
    printf("Sending back %s\n", string);
    if( (send(ClientSocket, (void *) string, strlen(string), 0)) < 0) //Send string back to client.
        printf("Failed to send\n");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  FUNCTION: XMLParser

    Parses an XML value and returns a token.

    @param begin           --      The expected beginning of an XML expression
    @param end             --      The expected ending of an XML expression
    @param token           --      The token extracted from the expression
    @param clientMessage   --      Message to parse
    @param length          --      Size of token
    @return                --      1 on success, 0 on failure, -1 if token is too large to fit
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int XMLParser(  const char* beginXml,
                const char* endXml,
                char* clientMessage,
                char* token,
                int tokenSize)
{
    //~~~~~~~~~~~~~Local vars ~~~~~~~~~~~~~~~~~~~~~~~//
    char tempString[strlen(clientMessage)];
    char *delimiter = NULL;
    int returnVal = 0;
    int i = 0;
    int beginXmlLength = strlen(beginXml);
    int endXmlLength = strlen(endXml);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    token[0] = '\0';
    memcpy(tempString, clientMessage, beginXmlLength); //Copy first part of clientMessage into temp for comparison.
    tempString[beginXmlLength] = '\0';
    if(strcmp(tempString, beginXml) == 0 ) //If beginXml is found
    {
        memcpy(tempString, clientMessage, strlen(clientMessage)); //Copy entire clientMessage
        for(i = 1; i < strlen(clientMessage); i++) //Check for valid delimiter here
        {
            if(tempString[i] == '<')
            {
                delimiter = tempString+i; //Potential valid delimiter found. Point delimiter ptr to location.
                break;
            }
        }
        delimiter[endXmlLength] = '\0';//Set end of delimiter to null
        if(strcmp(delimiter, endXml) != 0) //If invalid delimiter
            returnVal = 0;
        else
        {
            returnVal = 1;//Set valid return
            char *tempToken = clientMessage+(strlen(beginXml)); //Set temporary token to end of starting delimiter
            strtok(tempToken, "<");
            if(strlen(tempToken) > tokenSize ) returnVal = -1;              //If token is too large, return -1
            else if(strcmp(tempToken, endXml) == 0 ) token[0] = '\0';       //Else if empty token found
            else strcat(token, tempToken);                                  //Else put extracted token in variable
        }
    }
    return returnVal;
}