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
const int BUFFERSIZE = 256;

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
        printf("Usage:\n%s <port to open>\n",argv[0]); //TODO check for negative port number entry
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
    /*~~~~~~~~~~~~~~~~~~~~~Local vars~~~~~~~~~~~~~~~~~~~~~*/
    int ClientSocket = *(int*)ClientSocketPtr;
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
        ParseClientMessage(stringBuffer, ClientSocket);
    }
    close(ClientSocket);    /* Close client socket */
    free(ClientSocketPtr);
    pthread_exit(NULL);
}


//Parse message
int ParseClientMessage(char* clientMessage, int ClientSocket)
{

    int i = 0;
    char string[BUFFERSIZE]; //String to send back to client
    char errorString[BUFFERSIZE];
    errorString[0] = '\0'; //Set strings to empty
    string[0] = '\0';
    strcat(errorString, "<error>unknown format</error>");

    /*~~~~~~~~~~~~~~~~~~~~~Load avg response~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if(strcmp(clientMessage, "<loadavg/>") == 0)
    {
        double loadavg[3];
        getloadavg(loadavg, 3); //Get load average and put in double array

        //Begin string format:
        strcat(string, "<replyLoadAvg>");
        for(i = 0; i < 3; i++)
        {
            char tempAvg[BUFFERSIZE];
            sprintf(tempAvg, "%lf:", loadavg[i]); //Print double to string
            strcat(string, tempAvg);              //Append string to string we return to client
        }
        strcat(string, "</replyLoadAvg>"); //End of string
    }
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*~~~~~~~~~~~~~~~~~~~~~Echo response~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    else
    {
        char token[BUFFERSIZE];
        XMLParser("<echo>", "</echo>", clientMessage, token, sizeof(token));
        printf("token: %s\n", token);
        fflush(stdout);
    }
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    //TODO implement error
    //Syntax if unknown message
    //Return <error>unknown format</error>
    send(ClientSocket, (void *) string, sizeof(string), 0); //Send string back to client.
    return 0;
}

/*
    begin           --      The expected beginning of an XML expression
    end             --      The expected ending of an XML expression
    token           --      The token extracted from the expression
    clientMessage   --      Message to parse
    length          --      Size of token
    return          --      1 on success, 0 on failure, -1 if token is too large to fit
 */
int XMLParser(  const char* beginXml,
                const char* endXml,
                const char* clientMessage,
                char* token,
                int tokenSize)
{
    //~~~~~~~~~~~~~Comparison strings~~~~~~~~~~~~~~~~//
    char tempString[BUFFERSIZE];
    char *delimiter = NULL;
    int returnVal = 0;
    int i = 0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    token[0] = '\0'; //Empty string token

    memcpy(tempString, clientMessage, strlen(beginXml)); //Copy first part of clientMessage into temp for comparison.

    if(strcmp(tempString, beginXml) == 0 ) //If beginXml is found
    {
        memcpy(tempString, clientMessage, strlen(clientMessage)); //Copy entire clientMessage
        for(i = 1; i < strlen(clientMessage); i++) //Check for valid delimiter here
        {
            if(tempString[i] == '<') //I
            {
                delimiter = tempString+i; //Potential valid delimiter found. Point delimiter ptr to location.
                break;
            }

        }
        if(strcmp(delimiter, endXml) != 0) //Invalid delimiter
            returnVal = 0;
        else //We have a valid delimiter!
        {
            returnVal = 1;//Set valid return
            char *tempToken = clientMessage+(strlen(beginXml)); //Set temporary token to end of starting delimiter
            strtok(tempToken, "<");
            if(strlen(tempToken) > tokenSize ) returnVal = -1;              //If token is too large, return -1
                else if(strcmp(tempToken, endXml) == 0 ) token[0] = '\0';   //Else if empty token found
            else strcat(token, tempToken);                                  //Else put extracted token in variable
        }
    }
    return returnVal;
}