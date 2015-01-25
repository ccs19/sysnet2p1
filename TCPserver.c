#include <stdio.h>
#include <stdlib.h> //For getloadavg()
#include <netdb.h> //For gethostbyname()

//For socket(), listen(), bind(), send(), recv(), accept(), getsockname()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> //for memset


#include "TCPserver.h"


//Globals
//TODO Get rid of globals
int ServerSocket = 0;
struct hostent *HostByName = NULL;
struct sockaddr_in ServerAddress;

//Constants
const int HostNameMaxSize = 256;
const int MAXLISTENERS = 5;

//Steps
//1. Create server socket
//2. Display info about socket -- Host name, IP, Port
//3. Wait for client connection
//3.a Create new detached thread
//3.b Interpret request
//3.c Perform action depending on request. Refer to specifications.
//4. Child thread close socket and terminate


//THREAD
pthread_t DetachedThread;
pthread_attr_t ThreadAttribute;

int main(int argc, const char* argv[])
{
    if(argc != 2)
        printf("Usage:\n%s <port to open>\n",argv[0]);
    else
    {
        OpenSocket(atoi(argv[1])); //Open socket with port in arg vector 1

        AcceptConnections();
    }
    return 0;
}


//ServerSocket of type int
void OpenSocket(int port)
{
    char hostname[HostNameMaxSize];

    if( ( ServerSocket =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) <  0)  //If socket fails
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


void DisplayInfo()
{
    int i = 0;
    struct in_addr ipAddress;
    printf("Host Name: %s\n",HostByName->h_name);
    printf("IP:        ");
    while(HostByName->h_addr_list[i] != 0)
    {
       ipAddress.s_addr = *(u_long*)HostByName->h_addr_list[i++];
        printf("%s\n", inet_ntoa(ipAddress));
    }
    printf("Port:      %d\n", htons(ServerAddress.sin_port));
    fflush(stdout);
}

void CloseSocket()
{

}

void InitAddressStruct(int port)
{
    memset((void*)&ServerAddress, '0', (size_t)sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(port);

}

void BindSocketAndListen()
{
    if( ( bind(ServerSocket, (struct sockaddr *) &ServerAddress, (socklen_t)sizeof(ServerAddress)) )  < 0)
        ExitOnError("Failed to bind socket"); //If binding of socket fails
    if( (listen(ServerSocket, MAXLISTENERS)) < 0)//Maximum number of listeners.
        ExitOnError("Failed to listen");

}

void AcceptConnections()
{


    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    int* ClientSocket = NULL;
    struct sockaddr_in ClientAddress;
    unsigned int clientAddressSize = sizeof(ClientAddress);
    pthread_t DetachedThread;
    pthread_attr_t ThreadAttribute;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DisplayInfo();
    InitDetachedThread(&ThreadAttribute);
    for(;;)
    {
        ClientSocket = malloc(sizeof(int));
        printf("ClientSocke1: %d\n", *ClientSocket);
        if( (*ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddress, &clientAddressSize) ) < 0)
            ExitOnError("Error in accept()");
        pthread_create(&DetachedThread, &ThreadAttribute, (void*)HandleClientRequests, (void*)ClientSocket);
    }
}

void InitDetachedThread(pthread_attr_t* ThreadAttribute)
{
    pthread_attr_init(ThreadAttribute);
    pthread_attr_setdetachstate(ThreadAttribute, PTHREAD_CREATE_DETACHED);
}


void ExitOnError(char* errorMessage)
{
    printf("%s\n", errorMessage);
    exit(1);
}

void HandleClientRequests(void* ClientSocketPtr)
{
    int ClientSocket = *(int*)ClientSocketPtr;
    printf("ClientSocket2: %d\n", ClientSocket);
    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    const int BUFFERSIZE = 256;
    char stringBuffer[BUFFERSIZE];
    int msgSize = 0;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if( (msgSize = read(ClientSocket, stringBuffer, sizeof(stringBuffer))) < 0) //If read message fails
    {
        printf("Failed to read message from client\n");
    }
    else //Else parse message and do something.
    {
        stringBuffer[msgSize + 1] = '\0';
        printf("%s", stringBuffer);
        fflush(stdout);
        send(ClientSocket, (void *) stringBuffer, sizeof(stringBuffer), 0);
    }
    close(ClientSocket);    /* Close client socket */
    free(ClientSocketPtr);
    pthread_exit(NULL);
}


//Parse message
int ParseClientMessage(char* clientMessage)
{

    //TODO implement echo
    //Syntax <echo>string</echo>
    //When echo received, return <reply>string</reply>

    //TODO implement loadavg
    //Syntax </loadavg>
    //Returns <replyLoadAvg>avg:avg:avg</replyLoadAvg>

    //TODO implement error
    //Syntax if unknown message
    //Return <error>unknown format</error>

    return 0;
}