/*
 * @file led_driver_test.c:
 * @brief control an LED driven by a pin of the RPi GPIO
 *
 * uses the wiringPi RPi GPIO driver library 
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>

#include "led_driver.h"


// Test code

int main (int argc, char *argv[])
{
  int pin = 0;
  if  (argc < 2 ){
    printf("usage: blink <pin 0 - 7>\n");
    exit(1);
  }
  
  pin = atoi(argv[1]);
  if ((pin < 0) || (pin > 7)){
    printf("invalid pin number - must be in range 0 to 7\n");
    exit(1);
  }

  printf ("Raspberry Pi blink pin %d\n", pin) ;

  initLED();
  
  for(;;)
  {
    turnOnLED();
    delay(500);
    turnOffLED();
    delay(500);
  }
}
