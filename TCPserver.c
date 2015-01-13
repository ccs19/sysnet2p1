#include <stdio.h>
#include <stdlib.h> //For getloadavg()
#include <netdb.h> //For gethostbyname()

//For socket(), listen(), bind(), send(), recv(), accept(), getsockname()
#include <sys/socket.h>



#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>


//Globals
int listenSocket = 0;
struct hostent *HostByName = NULL;

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
void CreateSocket();
void DisplayInfo();





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
    CreateSocket();
    DisplayInfo();
    return 0;
}



void CreateSocket()
{
    char hostname[HostNameMaxSize];
    listenSocket =  socket(AF_INET, SOCK_STREAM, 0);    //Attempt to open socket
    if(listenSocket == -1) //If socket fails, exit
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

    herror("Error printed for reference: ");            //Check value of errno. Remove this when no longer needed
}


void DisplayInfo()
{
    printf("Host Name: %s\n",HostByName->h_name);

}