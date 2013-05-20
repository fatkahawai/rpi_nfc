/* 
 * @file client.h
 * @brief public interface of tcp_client.c
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */

// function prototypes
int  openTCPsocket( char *, int );
void closeTCPsocket( void );
int  readTCPmessage( char * , int );
int  writeTCPmessage( char * );


