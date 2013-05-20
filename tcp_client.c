/* 
 * @file tcp_client.c
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

#include "tcp_client.h"


// STATIC GLOBALS (referenceable within this file only) 
static int sockfd = -1;


// ---------------------------------------------------------------------------
// open a socket and connect to a remote server
//
// returns : 0 if OK, else error code
// 
int openTCPSocket(char *szHostName, int nPort ){

    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        return(1); // error("ERROR opening socket");
    server = gethostbyname( szHostName );
    if (server == NULL) {
        return(2); // error no such host
    }

    // connect to server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons( nPort );

    // try connect to server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        return(3); // error("ERROR connecting");

    return(0); // return OK
}

// ---------------------------------------------------------------------------
// send a TCP message to server
//
// returns: number of bytes written,  else < 1 on error
//
int sendTCPmessage( char *message ) {

    return( write(sockfd,message,strlen(message)) );
}

// ---------------------------------------------------------------------------
// read from the socket - wait for response from server
//
// returns: number of bytes read or < 1 on error. 
//
int readTCPmessage( char *buffer, int buflen ){

    bzero(buffer,buflen);
    return( read(sockfd,buffer,buflen-1) );
}

// ---------------------------------------------------------------------------
// close socket
//
void closeTCPsocket(void){
    if( sockfd >= 0 )    
      close(sockfd);
}


/* TEST CODE
// ---------------------------------------------------------------------------
// main
//
int main(int argc, char *argv[])
{
    int nPortNo, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    char *szHostName;

    // parse command line arguments
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    szHostName = argv[1];
    printf("opening TCP socket to %s:%d\n", szHostName, nPortNo );

    if ( (n= openTCPSocket( szHostName, nPortNo )) != 0 ){
      printf(stderr,"ERROR %d: ",n);
      switch(n){
        case 1: printf("unable to open a socket\n");
        case 2: printf("no such host\n");
        case 3: printf("unable to connect to server\n");
        default: printf("<unknown>\n");
      }
      exit(n);
    }

    // session. send TCP messages to server
    while(1){

    if( sendTCPmessage("Hello World!") <= 0 ){
        printf("ERROR writing to socket\n");
    else{
        if( readTCPmessage(&buffer) < 0 ){
            error("ERROR reading from socket");
        else
            printf("Received %d bytes from server: %s\n",n, buffer);
        }
    }
  

    closeTCPsocket();
}
*/
