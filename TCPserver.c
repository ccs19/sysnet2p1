#include <stdio.h>
#include <stdlib.h> //For getloadavg()
#include <netdb.h> //For gethostbyname()

//For socket(), listen(), bind(), send(), recv(), accept(), getsockname()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


//Globals
int ListenSocket = 0;
struct hostent *HostByName = NULL;
struct sockaddr_in *SocketAddress = NULL;

//Constants
const int HostNameMaxSize = 256;




//hostent struct info for reference
/*

struct hostent {
    char  *h_name;             official name of host
char **h_aliases;          alias list
int    h_addrtype;         host address type
int    h_length;           length of address
char **h_addr_list;        list of addresses
}
#define h_addr h_addr_list[0]  for backward compatibility

*/





//Prototypes.
//TODO Implement in header eventually
void OpenSocket();
void DisplayInfo();
void CloseSocket();
void InitAddressStruct();
void BindSocketAndListen();
void AcceptConnections();




//Steps
//1. Create server socket
//2. Display info about socket -- Host name, IP, Port
//3. Wait for client connection
//3.a Create new detached thread
//3.b Interpret request
//3.c Perform action depending on request. Refer to specifications.
//4. Child thread close socket and terminate


int main()
{
    OpenSocket();
    DisplayInfo();
    AcceptConnections();
    CloseSocket();
    return 0;
}



void OpenSocket()
{
    char hostname[HostNameMaxSize];
    ListenSocket =  socket(AF_INET, SOCK_STREAM, 0);    //Attempt to open socket
    if(ListenSocket == -1)                              //If socket fails, exit
    {
        printf("Error creating socket\n");
        exit(1);
    }
    if(gethostname(hostname, sizeof(hostname)) == -1)   //If getting hostname fails, exit
    {
        printf("Error acquiring hostname. Exiting\n");
        exit(1);
    }
    HostByName = gethostbyname(hostname);
    if(HostByName ==  NULL)                             //If gethostbyname fails print error message, exit
    {
        herror("GetHostByName failed. Error: \n");
        exit(1);
    }
    herror("Error printed for reference: ");            //Check value of errno. TODO Remove this when no longer needed
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
    printf("Port:      %d\n", 0); //TODO Fill in the blanks =-)

}

void CloseSocket()
{

}

void InitAddressStruct()
{
    SocketAddress = malloc(sizeof(struct sockaddr_in)); //TODO Free this struct
    memset((void*) &SocketAddress, 0, (size_t)sizeof(SocketAddress));
    SocketAddress->sin_family = (short)(AF_INET);
    memcpy((void*) &SocketAddress->sin_addr, (void*) &HostByName->h_addrtype, HostByName->h_length);
    SocketAddress->sin_port = htons((u_short)8000);
}

void BindSocketAndListen()
{
    //TODO Add error checking
    bind(ListenSocket, (struct sockaddr*) &SocketAddress, (socklen_t)sizeof(SocketAddress));
    listen(ListenSocket, 100); //Maximum number of listeners.TODO Change arbitrary number to constant
}

void AcceptConnections()
{

}