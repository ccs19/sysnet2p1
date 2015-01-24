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