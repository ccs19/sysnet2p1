#include <stdio.h>
#include <stdlib.h> //For getloadavg()
#include <netdb.h> //For gethostbyname()

//For socket(), listen(), bind(), send(), recv(), accept(), getsockname()
#include <sys/socket.h>



#include <sys/types.h>
#include <pthread.h>


//Globals
int listenSocket = 0;


//Steps
//1. Create server socket
//2. Display info about socket -- Host name, IP, Port
//3. Wait for client connection
//3.a Create new detached thread
//3.b Interpret request
//3.c Perform action depending on request. Refer to specifications.
//4. Child thread close socket and terminate


void CreateSocket();
void DisplayInfo();

int main()
{
    CreateSocket();
    return 0;
}



void CreateSocket()
{
    //char hostname[256];
    listenSocket =  socket(AF_INET, SOCK_STREAM, 0);
    //gethostbyname(hostname); //Note: this function resolves URL to IP address
    //printf("%s\n", hostname);
    printf("Socket %d", listenSocket);

}