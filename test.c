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
void setLEDInterval ( long int lMilliseconds ){
    setInterval( LED_TIMER, lMilliseconds );
}

// ---------------------------------------------------------------------------
// set timer for async LED blink delay
// 
void setNFCInterval ( long int lMilliseconds ){
    setInterval( NFC_TIMER, lMilliseconds );
}

// ---------------------------------------------------------------------------
// set timer for async LED blink delay
// 
void setTCPInterval ( long int lMilliseconds ){
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

// ===========================================================================
// main
//
int main(int argc, char *argv[])
{
    long int nL, nT, nN;
    int i;

    // parse command line arguments
    if (argc < 4) {
       printf("usage %s LED(0) NFC(1) TCP(2) [delay in millisecs]\n", argv[0]);
       exit(0);
    }
    nL = atol(argv[1]);
    nN = atol(argv[2]);
    nT = atol(argv[3]);

    setInterval( LED_TIMER, nL );
    setInterval( NFC_TIMER, nN );
    setInterval( TCP_TIMER, nT );

    while(1){
        for( i=0 ; i < 3 ; i++ ){
            if( intervalTimeIsUp( i ) ){
                printf("timer %d is up\n", i );
            }
        }
    }

} // main()

