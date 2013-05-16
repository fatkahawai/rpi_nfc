/* 
 * SERVER.C
 * A simple server using TCP via sockets
 * The port number is passed as an argument
 *   
 * The server waits for connections from a client, then pushes messages over the socket
 *
 * (C) 2013 Fatkahawai
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256], wbuffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int i, n;
     struct timeval tod1, tod2;


     if (argc < 2) {
         fprintf(stderr,"usage: port\n");
         exit(1);
     }
     if((portno = atoi(argv[1])) == 0) 
         error("input error - invalid port number (0)");

     printf("starting socket server on port %d\n",portno);

     // create a TCP socket
     sockfd = socket(AF_INET,     // Internet protocol family
                     SOCK_STREAM, // use TCP (use SOCK_DGRAM for UDP) 
                     0);          // default protocol
     if (sockfd < 0) 
        error("failed to open socket");


     // assign an address to the socket.
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;    //   = "use my IP address"
     serv_addr.sin_port = htons(portno);        //   TCP/IP port number

     if (bind(sockfd, (struct sockaddr *) &serv_addr,  
              sizeof(serv_addr)) < 0) 
              error("failed to bind to socket");

     // listen on socket for connection reqeusts from clients    
     listen(sockfd,5);
     printf("Listening for connection requests from clients\n");

     // blocking call to accept a connection request from client
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("failed accepting connection request from client");

     char str[INET_ADDRSTRLEN];
     inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);

     printf("accepted connection request from client at %s:%d\n", str, portno); // ntohs(cli_addr.sin_port));

     // set socket to non-blocking
     if( fcntl( sockfd, F_SETFL, O_NONBLOCK ) < 0 )
        error("failed to set socket to non-blocking");


     gettimeofday(&tod1, NULL);  //set timer reference point to now

     while( 1 ){

      char message[255];

      // non-blocking read of socket
    //  bzero(buffer,256);
//      if( rcv( newsockfd, buffer, 255, MSG_DONTWAIT) < 0 )
         // nothing in socket in buffer

       
       // check if the client sent us a message
       bzero(buffer,256);
       n = read(newsockfd,buffer,255);
       if (n < 0) error("ERROR reading from socket");       
       if( n > 0 ) {
        printf("Received polling request %d (%d bytes): %s\n", i, n, buffer);

        printf("Sending ACK to client\n");
        n = write(newsockfd,"ACK",3);
        if (n < 0) error("ERROR writing to socket");
       }

       // Send a push message to the client every 3sec

       gettimeofday(&tod2, NULL); 
       if ((( tod2.tv_sec - tod1.tv_sec )/ 20) == 0 ){
         gettimeofday(&tod1, NULL); // reset reference to now

         if (n < 0) error("ERROR reading from stdin");       
         if ( n > 0) {
           sprintf(wbuffer,"Hello World! (Msg No. %d)",i++);

           printf("Sending push message to client [%s]\n", wbuffer);

           n = write(newsockfd,wbuffer,strlen(wbuffer));
           if (n < 0) error("ERROR writing to socket");
         }
       }

     }

     // close socket
     close(newsockfd);
     close(sockfd);
     printf("Disconnected\n");

     return 0; 
}
