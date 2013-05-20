/*
 * @file nfc_driver.c
 * @brief Polling the RPi NFC board
 *
 * uses the libnfc platform independent Near Field Communication (NFC) library 
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com> 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif // HAVE_CONFIG_H

#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "nfc.h"
#include "nfc-types.h"
#include "nfc-utils.h"

#include "nfc_driver.h"

// Definitions
#define MAX_DEVICE_COUNT 16

// STATIC GLOBALS (referenceable within this file only) 
static nfc_device *pnd = NULL;

// ---------------------------------------------------------------------------
// stop polling the NFC board
//
void stop_polling (int sig)
{
  (void) sig;
  if (pnd)
    nfc_abort_command (pnd);
  else
    exit (EXIT_FAILURE);
  printf("\nNFC polling aborted\n");
}

// ---------------------------------------------------------------------------
// Initialize NFC device 
//
// returns: 0 if OK, else -1 
//
int initNFC(void){
  nfc_init (NULL);

  pnd = nfc_open (NULL, NULL);

  if (pnd == NULL) {
    ERR ("%s", "ERROR: Unable to open NFC device.");
    return(-1); // exit (EXIT_FAILURE);
  }

  if (nfc_initiator_init (pnd) < 0) {
    nfc_perror (pnd, "nfc_initiator_init");
    return(-1); // exit (EXIT_FAILURE);    
  }

  // Enable field so more power consuming cards can power themselves up
  // nfc_configure (pnd, NDO_ACTIVATE_FIELD, true);

  printf ("NFC reader: %s opened\n", nfc_device_get_name (pnd));

  if (signal (SIGINT, stop_polling) == SIG_ERR)   // set interupt handler on Ctl-C 
    perror("ERROR: can't catch SIGINT");
  
  // Display libnfc version
  const char *acLibnfcVersion = nfc_version ();

  printf ("using libnfc %s\n", acLibnfcVersion);

  return(0);
} // initNFC

// ---------------------------------------------------------------------------
// poll NFC device for transactions
// 
// returns: result
//
int pollNFC( nfc_target *pTarget , int nPolls, int nInterval ){
  int res = 0;
  uint8_t uiPollNr; // number of time to poll
  uint8_t uiPeriod;  // time between polls in 150ms units
  const nfc_modulation nmModulations[5] = { // list of tag types you will accept
    { .nmt = NMT_ISO14443A, .nbr = NBR_106 },
    { .nmt = NMT_ISO14443B, .nbr = NBR_106 },
    { .nmt = NMT_FELICA, .nbr = NBR_212 },
    { .nmt = NMT_FELICA, .nbr = NBR_424 },
    { .nmt = NMT_JEWEL, .nbr = NBR_106 },
  };
  const size_t szModulations = 5; // number of tag types in the list

  uiPollNr = (uint8_t )nPolls;
  uiPeriod = (uint8_t )nInterval;

  printf ("NFC device will poll during %ld s (%u pollings of %lu ms for %zd modulations)\n", (unsigned long) (uiPollNr * szModulations * uiPeriod * 150)/1000, uiPollNr, (unsigned long) uiPeriod * 150, szModulations);

  if ((res = nfc_initiator_poll_target (pnd, nmModulations, szModulations, uiPollNr, uiPeriod, pTarget))  < 0) {
    nfc_perror (pnd, "nfc_initiator_poll_target");
    // nfc_close (pnd);
    // nfc_exit (NULL);
    return(-1); // exit (EXIT_FAILURE);
  }
  return(res);
} // pollNFC

// ---------------------------------------------------------------------------
// close NFC device 
//
void closeNFC( void ){
  if( pnd != NULL )
    nfc_close (pnd);
  nfc_exit (NULL);
}

/*
// ===========================================================================
// example main
//
int main (int argc, const char *argv[])
{
  bool verbose = false;
  int res = 0;

  if (argc != 1) {
    if ((argc == 2) && (0 == strcmp ("-v", argv[1]))) {
      verbose = true;
    } else {
      printf ("usage: %s [-v]\n", argv[0]);
      printf ("  -v\t verbose display\n");
      exit (EXIT_FAILURE);
    }
  }
  
  if( initNFC() < 0) {
    exit(EXIT_FAILURE);
  }
  if ( verbose )
    printf("NFC device Initialized successfully.");

  nfc_target nt;
  res = pollNFC( &nt );

  // display results from NFC target device
  if (res > 0) {
    print_nfc_target ( nt, verbose );
  } else if (res < 0){
    printf("polling failed.\n");
  } else {
    printf ("No NFC target found.\n");
  }

  // close NFC device and end program
  closeNFC();
  exit (EXIT_SUCCESS);
}*/
