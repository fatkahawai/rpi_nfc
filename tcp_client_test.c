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

#define LIST_SIZE 5

char *szJSONstringList[] = {
    "{\"nfcModulationType\":\"ISO/IEC 14443-a\",\"baudRate\":\"100\",\"ATQA\":\"0\",\"UID\":\"01 FF FF FF\"}",
    "{\"nfcModulationType\":\"ISO/IEC 14443-b\",\"baudRate\":\"64\",\"ATQA\":\"1\",\"UID\":\"02 FF FF FF\"}",
    "{\"nfcModulationType\":\"ISO/IEC 14443-c\",\"baudRate\":\"64000\",\"ATQA\":\"2\",\"UID\":\"03 FF FF FF\"}",
    "{\"nfcModulationType\":\"ISO/IEC 14443-d\",\"baudRate\":\"1024\",\"ATQA\":\"3\",\"UID\":\"04 FF FF FF\"}",
    "{\"nfcModulationType\":\"ISO/IEC 14443-e\",\"baudRate\":\"8\",\"ATQA\":\"4\",\"UID\":\"05 FF FF FF\"}",
};

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
    time_t tStart, tNow;
    double fdElapsedSeconds;

    time(&tStart);  /* get current time; same as: timer = time(NULL)  */

    do{ 
        time( &tNow );
        fdElapsedSeconds = difftime( tNow, tStart );
    } while ( fdElapsedSeconds < nDelaySeconds );
    
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
    int nPortNo, n, res, i;
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
        case 1: fprintf(stderr,"unable to open a socket\n"); break;
        case 2: fprintf(stderr,"no such host\n"); break;
        case 3: fprintf(stderr,"unable to connect to server\n"); break;
        default: fprintf(stderr,"<unknown>\n");
      }
      exit(n);
    } // if    
    printf("Socket opened. Sending messages to server...(hit Ctl-C to stop)\n");

    // session. send TCP messages to server
    for( i=0 ; i < LIST_SIZE ; i++ ){

        printf("Sending %s\n", szJSONstringList[i]);
        if( sendTCPmessage(szJSONstringList[i]) <= 0 ){
            fprintf(stderr,"ERROR writing to socket\n");
            exit(1);
        }
        else{
            if( (res= readTCPmessage(buffer, 256)) < 0 ){
                fprintf(stderr,"ERROR reading from socket\n");
                exit(2);
            } else if ( res > 0 )
                printf("Received %d bytes from server: %s\n", res, buffer);
        } // else
        delay(1); // pause for a second
 
    } // while(1)
    closeTCPsocket();
} // main()

