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


#define BUFFER_SIZE         1024         // size of TCP write and read buffer in bytes
#define NFC_POLL_INTERVAL   1000         // pause 1sec between NFC device poll attempts
#define LED_ON_INTERVAL      500         // turn LED on for 500ms 
#define TCP_TIMEOUT         5000         // timeout waiting for ACK from server 


// ---------------------------------------------------------------------------
// static variables for the async delay timer functions
//
typedef enum{
    LED_TIMER = 0,
    NFC_TIMER,
    TCP_TIMER } timer_type;

static struct timeval   timeRef[3];
static long int         lInterval[3];
static long int         lNextTriggerTime[3];


// ---------------------------------------------------------------------------
// set timer for async delay
// 
void setInterval (  timer_type eTimerID, long int lMilliseconds ){
    long int lStartTime;

    gettimeofday( &timeRef[eTimerID], NULL );
    lStartTime = (double) (timeRef[eTimerID].tv_sec * 1000); // secs x 1000 = millisecs
    lStartTime+= (double) (timeRef[eTimerID].tv_usec / 1000); // microsecs/1000 = millisecs

    lInterval[eTimerID] = lMilliseconds;
    lNextTriggerTime[eTimerID] = lStartTime + lInterval[eTimerID];
}

// ---------------------------------------------------------------------------
// set timer for async LED blink delay
// 
void setLEDinterval ( long int lMilliseconds ){
    setInterval( LED_TIMER, lMilliseconds );
}

// ---------------------------------------------------------------------------
// set timer for async LED blink delay
// 
void setNFCinterval ( long int lMilliseconds ){
    setInterval( NFC_TIMER, lMilliseconds );
}

// ---------------------------------------------------------------------------
// set timer for async LED blink delay
// 
void setTCPtimeout ( long int lMilliseconds ){
    setInterval( TCP_TIMER, lMilliseconds );
}

// ---------------------------------------------------------------------------
// async delay - check if the interval time has elapsed 
// 
// returns true if time is up, else false
//
bool intervalTimeIsUp(  timer_type eTimerID ){

    struct timeval timeNow;
    long int lCurrentTime;

    gettimeofday( &timeNow, NULL );
    lCurrentTime = (double) (timeNow.tv_sec * 1000); // secs x 1000 = millisecs
    lCurrentTime+= (double) (timeNow.tv_usec / 1000); // microsecs/1000 = millisecs

    // printf("lCurrentTime= %ld\n", lCurrentTime );

    if( lCurrentTime >= lNextTriggerTime[eTimerID] ){
        lNextTriggerTime[eTimerID] = lCurrentTime + lInterval[eTimerID];
        return( true);
    }
    else
        return( false );
}

// ---------------------------------------------------------------------------
// Error handler
// close NFC device and TCP socket, turn off LED, and exit
//
void error(const char *msg)
{
    perror(msg);
    closeTCPsocket();
    closeNFC();

    turnOffLED();
    exit(0);
}

// ---------------------------------------------------------------------------
// delay
// 
void delay ( int nDelayMilliSeconds )
{  
    time_t start, now;
    double elapsedSeconds;
    int nDelaySeconds = nDelayMilliSeconds/1000; 

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
    setLEDinterval( LED_ON_INTERVAL ); // blink LED     


    setNFCinterval( NFC_POLL_INTERVAL ); 
    setTCPtimeout( TCP_TIMEOUT );   

    // session. send TCP messages to server
    while(1){

      if( isLEDon() )
        if( intervalTimeIsUp(LED_TIMER) )
            turnOffLED();

      if( intervalTimeIsUp( NFC_TIMER) ) {
        // make one poll attempt of NFC device to detect any target
        if( (res= pollNFC( &nfcTarget, 1, 1 )) < 0 ) {
            error("polling NFC device failed");
        } 
        else if ( res > 0 ) {
            // display results from NFC target device
            print_nfc_target ( nfcTarget, true );

            // blink LED
            turnOnLED();
            setLEDinterval( LED_ON_INTERVAL );

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
      } // if
        //delay( POLL_INTERVAL ); // wait half a second before polling again
    } // while(1)


    turnOffLED();
    closeTCPsocket();
    closeNFC();

} // main()

