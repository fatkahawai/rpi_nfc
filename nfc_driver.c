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

// local function prototypes
static int stringify_nfc_iso14443a_info (const nfc_iso14443a_info nai, char *szBffer );

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

  printf("\nNFC polling aborted\n");
  if (pnd)
    nfc_abort_command (pnd);
  exit (EXIT_FAILURE);
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

  // printf ("NFC device will poll for %ld ms (%u pollings of %lu ms for %zd modulations)\n", (unsigned long) (uiPollNr * szModulations * uiPeriod * 150), uiPollNr, (unsigned long) uiPeriod * 150, szModulations);

  if ((res = nfc_initiator_poll_target (pnd, nmModulations, szModulations, uiPollNr, uiPeriod, pTarget))  < 0) {
    if( res == -90 )
      res = 0; // return code signifying no target found
    else{
      nfc_perror (pnd, "nfc_initiator_poll_target");
      fprintf(stderr,"result %d", res);
      // nfc_close (pnd);
      // nfc_exit (NULL);
    }
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


// ---------------------------------------------------------------------------
// create JSON-encoded nfc_target struct in buffer to send to server
//
// returns : number of chars written into buffer
//
int constructJSONstringNFC( const nfc_target nfcTarget, char *szBuffer, int nBufLen ){

  char element[256];

  bzero(szBuffer, nBufLen);

  sprintf(szBuffer, "{" );

  // nfc_modulation_type
  // and baudRate
  //
  switch(nfcTarget.nm.nmt) {
    case NMT_ISO14443A:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "ISO/IEC 14443A");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      stringify_nfc_iso14443a_info (nfcTarget.nti.nai, element);
      strcat(szBuffer, element);

    break;
    case NMT_JEWEL:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "Innovision Jewel");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_jewel_info (nfcTarget.nti.nji, verbose);
    break;
    case NMT_FELICA:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "FeliCa");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_felica_info (nfcTarget.nti.nfi, verbose);
    break;
    case NMT_ISO14443B:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "ISO/IEC 14443-4B");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_iso14443b_info (nfcTarget.nti.nbi, verbose);
    break;
    case NMT_ISO14443BI:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "ISO/IEC 14443-4Bi");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_iso14443bi_info (nfcTarget.nti.nii, verbose);
    break;
    case NMT_ISO14443B2SR:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "ISO/IEC 14443-2B ST SRx");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_iso14443b2sr_info (nfcTarget.nti.nsi, verbose);
    break;
    case NMT_ISO14443B2CT:
      sprintf(element, "\"nfcModulationType\":\"%s\"", "ISO/IEC 14443-2B ASK CTx");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_iso14443b2ct_info (nfcTarget.nti.nci, verbose);
    break;
    case NMT_DEP:
      
      sprintf(element, "\"nfcModulationType\":\"%s %s\"", "D.E.P.",
                       (nfcTarget.nti.ndi.ndm == NDM_ACTIVE)? "active mode" : "passive mode");
      strcat(szBuffer, element);

      sprintf(element, ",\"baudRate\":\"%s\"",str_nfc_baud_rate(nfcTarget.nm.nbr) );
      strcat(szBuffer, element);

      //stringify_nfc_dep_info (nfcTarget.nti.ndi, verbose);
    break;
  }
  strcat(szBuffer, "}" );
  return( strlen(szBuffer) );
}

// ---------------------------------------------------------------------------
// convert a sequence of bytes into a Hex string
//
//
void stringifyToHex(char *szBuffer, const uint8_t *pbtData, const size_t szBytes )
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++)
    sprintf(szBuffer, "%02X-", pbtData[szPos]); 
}
// ---------------------------------------------------------------------------
// Internal function to JSON-encode the ISO 14443A structure 
//
// returns : number of characters written into buffer
//
static int stringify_nfc_iso14443a_info (const nfc_iso14443a_info nai, char *szBuffer )
{
  char szElement[256];
  unsigned long ulUID;
  int i;

  // ATQA (Answer to Request)
  strcat( szBuffer,",\"ATQA\":\"" );
  stringifyToHex(szElement, nai.abtAtqa , 2 );
  strcat( szBuffer, szElement );
  strcat( szBuffer,"\"" );
 
  // UID (Unique Identifier)
  strcat(szBuffer, ",\"UID\":\"");
  stringifyToHex(szElement, nai.abtUid, nai.szUidLen );

  strcat( szBuffer, szElement );
  if ( nai.abtUid[0] == 0x08 )
    strcat( szBuffer," (Random UID)");
  strcat(szBuffer, "\"" );

  // ATS (Answer to Select)
  // TODO if required

  return( strlen( szBuffer) );
}

