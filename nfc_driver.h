
/*
 * @file nfc_driver.h
 * @brief Public Interface to nfc_driver.c
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com> 
 */



// Function prototypes
int  initNFC( void );
int  pollNFC( nfc_target *nt , int nPolls, int nInterval );
void closeNFC( void );
