/* 
 * @file tcp_client_test.c
 * @brief A simple client using TCP via sockets
 *
 * The host and port number of the server is passed as an argument
 *   
 * The client connects to that remote server, then sends messages over the socket
 * and listens for an ACK from server.
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

#include "tcp_client.h"


// Error handler
//
void error(const char *msg)
{
    perror(msg);
    closeTCPsocket();

    exit(0);
}

// delay
//
void delay ( int nDelaySeconds )
{  
    time_t start, now;
    double elapsedSeconds;

    time(&start);  /* get current time; same as: timer = time(NULL)  */

    do{ 
        time( &now );
        elapsedSeconds = difftime( now, start );
    } while ( elapsedSeconds < nDelaySeconds );
}

// ===========================================================================
// main
//
// Commandline arguments:
// argv[1]  Host name of server to connect to
// argv[2]: port number 
//
int main(int argc, char *argv[])
{
    int nPortNo, n, res;
    char buffer[256];
    char *szHostName;

    // parse command line arguments
    if (argc < 3) {
       printf("usage %s hostname port\n", argv[0]);
       exit(0);
    }
    nPortNo = atoi(argv[2]);
    szHostName = argv[1];
    printf("opening TCP socket to %s:%d\n", szHostName, nPortNo );

    if ( (n= openTCPSocket( szHostName, nPortNo )) != 0 ){
      fprintf(stderr,"ERROR %d: ",n);
      switch(n){
        case 1: fprintf(stderr,"unable to open a socket\n");
        case 2: fprintf(stderr,"no such host\n");
        case 3: fprintf(stderr,"unable to connect to server\n");
        default: fprintf(stderr,"<unknown>\n");
      }
      exit(n);
    } // if

    // session. send TCP messages to server
    while(1){

        if( sendTCPmessage("Hello World!") <= 0 ){
            fprintf(stderr,"ERROR writing to socket\n");
            exit(1);
        }
        else{
            if( (res= readTCPmessage(buffer, 256)) < 0 ){
                fprintf(stderr,"ERROR reading from socket");
                exit(2);
            } else if ( res > 0 )
                printf("Received %d bytes from server: %s\n",n, buffer);
        } // else

        delay(1); // wait a second
 
    } // while(1)

    closeTCPsocket();
} // main()

