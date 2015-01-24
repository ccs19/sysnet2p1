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
int ClientSocket = 0;
struct hostent *HostByName = NULL;
struct sockaddr_in ServerAddress;
struct sockaddr_in ClientAddress;
char buffer[256];

//Constants
const int HostNameMaxSize = 256;
const int MaxStringLength = 256;
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

int main()
{
    OpenSocket();
    //InitDetachedThread();
    AcceptConnections();
    //pthread_create(&DetachedThread, &ThreadAttribute, (void *) AcceptConnections, NULL);
    //free(ServerSocket); //Temporary free
    sleep(500);
    return 0;
}


//ServerSocket of type int
void OpenSocket()
{
    char hostname[HostNameMaxSize];

    if( ( ServerSocket =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) <  0)//If socket fails
        ExitOnError("Error creating socket");

    if(gethostname(hostname, sizeof(hostname)) < 0)   //If getting hostname fails
        ExitOnError("Error acquiring hostname. Exiting");

    if( ( HostByName = gethostbyname(hostname) ) ==  NULL)                             //If gethostbyname fails print error message, exit
    {
        herror("GetHostByName failed. Error: ");
        exit(1);
    }

    InitAddressStruct();
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

void InitAddressStruct()
{
    memset((void*)&ServerAddress, '0', (size_t)sizeof(ServerAddress));
    //memcpy((void*)&ServerSocket.sin_addr, (void*) &HostByName->h_addrtype, HostByName->h_length);
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddress.sin_port = htons(40000);

}

void BindSocketAndListen()
{
    if( ( bind(ServerSocket, (struct sockaddr *) &ServerAddress, (socklen_t)sizeof(ServerAddress)) )  < 0)
        ExitOnError("Failed to bind socket"); //If binding of socket fails
    if( (listen(ServerSocket, MAXLISTENERS)) < 0)//Maximum number of listeners.TODO Change arbitrary number to constant
        ExitOnError("Failed to listen");

}

void AcceptConnections()
{
    DisplayInfo();

    unsigned int clientAddressSize = sizeof(ClientAddress);
    for(;;)
    {

        if( (ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddress, &clientAddressSize) ) < 0)
            ExitOnError("Error in accept()");
        printf("Connected! Accepted client %s\n", inet_ntoa(ClientAddress.sin_addr));

        close(ClientSocket);    /* Close client socket */
    }
}

void InitDetachedThread()
{
    pthread_attr_init(&ThreadAttribute);
    pthread_attr_setdetachstate(&ThreadAttribute, PTHREAD_CREATE_DETACHED);
}


void ExitOnError(char* errorMessage)
{
    printf("%s\n", errorMessage);
    exit(1);
}

