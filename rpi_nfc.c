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
#define NFC_QUARANTINE_INTERVAL 5000     // don't accept tx from same card within 5s

// ---------------------------------------------------------------------------
// static variables for the async delay timer functions
//
typedef enum{
    LED_TIMER = 0,
    NFC_TIMER,
    TCP_TIMER,
    QUA_TIMER } timer_type;

// interval registers. one arrary element for each enum timer_type
static struct timeval   timeRef[4];    
static long int         lInterval[4];
static long int         lNextTriggerTime[4];


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
    char szPrevBuffer[BUFFER_SIZE];
    char szReadBuffer[BUFFER_SIZE];
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

    strcpy(szBuffer, "" );
    strcpy(szPrevBuffer, "" );

    // session. send TCP messages to server
    while(1){

      if( isLEDon() )
        if( intervalTimeIsUp(LED_TIMER) )
            turnOffLED();

      if( intervalTimeIsUp( NFC_TIMER) ) {

        // make one poll attempt of NFC device to detect any target
        if( (res= pollNFC( &nfcTarget, 1, 1 )) < 0 ) {
            fprintf(stderr,"Non-fatal error - polling NFC device failed");
            continue;
        } 

        if( res == 0 )  // no target card detected
            continue;

        // a target card was detected, process the transaction

        // print detailed results from NFC target device to console
        print_nfc_target ( nfcTarget, true );

        // convert into a JSON string
        if( constructJSONstringNFC( nfcTarget, szBuffer, BUFFER_SIZE ) <= 0 ){
            fprintf(stderr,"Non-fatal Error - construct JSON string failed");
            continue;
        }

        // if its the same target detected again within the quarantine period, 
        // ignore it - don't send it to the server
        if(  strcmp( szBuffer, szPrevBuffer ) == 0 ){ // same card again
            if( !intervalTimeIsUp(QUA_TIMER) ){  // its still within quarantine period
                fprintf(stderr,"found same target card within quarantine period. ignoring it.\n");
                continue;
            }
        } 
        // save the JSON string to compare with the next card event, 
        // so we dont double-scan a card
        strcpy(szPrevBuffer, szBuffer );
        setInterval( QUA_TIMER, NFC_QUARANTINE_INTERVAL );

        printf("\nSending JSON: %s\n", szBuffer );

        // send JSON string as TCP message to the server
        if( sendTCPmessage( szBuffer ) <= 0 ){
            fprintf(stderr,"Non-fatal Error writing to socket. retrying\n");
            continue;
        }   

        // blink LED to acknowledge successfully recorded transaction to user
        turnOnLED();
        setLEDinterval( LED_ON_INTERVAL );

        // wait for ACK from server
        // NB socket in tcp_client.c is NOT initialized for non-blocking read !
        // setInterval( TCP_TIMER, TCP_TIMEOUT );
        /* TODO: sometimes after a period of downtime, the RPi doesnt get the ACK 
           so hangs in the read function. so i've disabled reading the ACK.
        if( (n= readTCPmessage(szReadBuffer, BUFFER_SIZE)) < 0 ){
            fprintf(stderr,"Non-fatal Error reading from socket. (ACK not received). retrying.\n");
            continue;
        }            
        else if( strcmp(&szReadBuffer[3], "{\"msg\":\"ACK\"}") == 0 )
            fprintf(stderr, "ACK received from server\n");
        else 
            fprintf(stderr, "Non-fatal Error - expected 'ACK' msg, but received %d bytes from server: %s\n",
                    n, szReadBuffer);
        */
        } // if NFC poll interval time is up - poll NFC device

    } // while(1)


    turnOffLED();
    closeTCPsocket();
    closeNFC();

} // main()

