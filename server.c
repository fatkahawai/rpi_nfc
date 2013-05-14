/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
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
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int i, n;
     int number_of_responses;

     if (argc < 3) {
         fprintf(stderr,"usage: server port number_of_responses\n");
         exit(1);
     }
     portno = atoi(argv[1]);
     number_of_responses = atoi(argv[2]);
     printf("starting socket server on port %d to handle max %d requests\n",portno,number_of_responses);

     // create a TCP socket
     sockfd = socket(AF_INET,     // Internet protocol family
                     SOCK_STREAM, // use TCP (use SOCK_DGRAM for UDP)
                     0);          // default protocol
     if (sockfd < 0) 
        error("ERROR opening socket");

     // assign an address to the socket.
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;    //   = "use my IP address"
     serv_addr.sin_port = htons(portno);        //   TCP/IP port number

     if (bind(sockfd, (struct sockaddr *) &serv_addr,  
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

     // listen on socket for connection reqeusts from clients    
     listen(sockfd,5);
     printf("Listening for connection requests from clients\n");

     // blocking call to accept a connection request from client
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR accepting connection request from client");

     char str[INET_ADDRSTRLEN];
     inet_ntop(AF_INET, &(cli_addr.sin_addr), str, INET_ADDRSTRLEN);

     printf("accepted connection request from client at %s:%d\n", str, portno); // ntohs(cli_addr.sin_port));

     for( i=0 ; i < number_of_responses ; i++ ){
       // wait for the client to send us a message
       bzero(buffer,256);
       n = read(newsockfd,buffer,255);
       if (n < 0) error("ERROR reading from socket");
       
       if( n == 0 ) {
         printf("Received zero-length message, ignoring\n");
         continue;
       }
       // display received message
       printf("Received request %d (%d bytes): %s\n", i, n, buffer);

       char responseStr[80];
       if( i < (number_of_responses) )
         sprintf(responseStr,"RESPONSE %d",i);
       else
         sprintf(responseStr,"CLOSE");

       printf("Sending response [%s]\n", responseStr);

       n = write(newsockfd,responseStr,strlen(responseStr));
       if (n < 0) error("ERROR writing to socket");
     }

     // close socket
     close(newsockfd);
     close(sockfd);
     printf("Disconnected\n");

     return 0; 
}
