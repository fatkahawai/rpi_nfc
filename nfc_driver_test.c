/*
 * @file nfc_driver_test.c
 * @brief Polling the RPi NFC board using nfc_driver.c
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

#include "nfc-types.h"

#include "nfc_driver.h"

#define BUFSIZE 256

// ===========================================================================
// main
//
int main (int argc, const char *argv[])
{
  bool verbose = false;
  int res = 0;
  char buffer[BUFSIZE];

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
  res = pollNFC( &nt, 20, 2 ); // do 20 polls at 2s intervals

  // display results from NFC target device
  if (res > 0) {
    print_nfc_target ( nt, verbose );

    printf("as JSON string: ", constructJSONstringNFC(nt, buffer, BUFSIZE));

  } else if (res < 0){
    printf("polling failed.\n");
  } else {
    printf ("No NFC target found.\n");
  }

  // close NFC device and end program
  closeNFC();
  exit (EXIT_SUCCESS);
}
