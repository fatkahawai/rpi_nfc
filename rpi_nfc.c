/* 
 * @file rpi_nfc.c
 * @brief An application for Raspberry Pi sending NFC data to a server via TCP socket
 *
 * The host and port number of the server is passed as an argument
 *   
 * The client connects to that remote server, then sends NFC transaction data over 
 * the socket and listens for an ACK from server. 
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "tcp_client.h"
#include "led_driver.h"
#include "nfc_driver.h"


#define BUFFER_SIZE     1024         // size of TCP write and read buffer in bytes
#define POLL_INTERVAL      1         // pause 1sec between NFC device poll attempts
#define LED_ON_INTERVAL    1         // turn LED on for 1sec 

// Error handler
//
void error(const char *msg)
{
    perror(msg);
    closeTCPsocket();
    closeNFC();

    turnOffLED();
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
    char szBuffer[BUFFER_SIZE];
    char *szHostName;
    nfc_target nfcTarget;

    // parse command line arguments
    if (argc < 3) {
       printf("usage %s hostname port\n", argv[0]);
       exit(0);
    }
    nPortNo = atoi(argv[2]);
    szHostName = argv[1];


    // Open Socket
    printf("opening TCP socket to %s:%d\n", szHostName, nPortNo );

    if ( (n= openTCPSocket( szHostName, nPortNo )) != 0 ){
      switch(n){
        case 1: error("unable to open a socket");
        case 2: error("no such host");
        case 3: error("unable to connect to server");
        default: error("<unknown>");
      }
    } // if

    // Init NFC device 
    if( initNFC() != 0 )
        error("unable to initialise NFC device");

    // Init GPIO for LED display
    if( initLED() != 0 )
        error("unable to initialise GPIO for LED display");
                // blink LED
    turnOnLED();
    delay( LED_ON_INTERVAL ); // blink LED for a second     
    turnOffLED();


    // session. send TCP messages to server
    while(1){

        // make one poll attempt of NFC device to detect any target
        if( (res= pollNFC( &nfcTarget, 1, 1 )) < 0 ) {
            error("polling NFC device failed");
        } 
        else if ( res > 0 ) {
            // display results from NFC target device
            print_nfc_target ( nfcTarget, true );

            // blink LED
            turnOnLED();

            if( constructJSONstringNFC( nfcTarget, szBuffer, BUFFER_SIZE ) <= 0 )
                error("construct JSON string failed");

            printf("\nSending JSON: %s\n", szBuffer );

            if( sendTCPmessage( szBuffer ) <= 0 ){
                fprintf(stderr,"ERROR writing to socket. retrying\n");
            } else{ // get the ACK from the server

                delay( LED_ON_INTERVAL ); // wait for ACK then turn LED off
                turnOffLED();

                if( (n= readTCPmessage(szBuffer, BUFFER_SIZE)) < 0 ){
                    fprintf(stderr,"ERROR reading from socket. retrying\n");
                } else {
                    if( strcmp(szBuffer, "ACK") == 0 )
                        printf("ACK received from server\n");
                    else printf("ERROR - expected 'ACK'. Received %d bytes from server: %s\n",
                                n, szBuffer);
                } // else read OK
            } // else sent OK
        } // else if res > 0 - we polled a target
        // else res ==0, i.e. no target detected, just continue

        delay( POLL_INTERVAL ); // wait half a second before polling again
    } // while(1)


    turnOffLED();
    closeTCPsocket();
    closeNFC();

} // main()

