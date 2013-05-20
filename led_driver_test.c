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
    printf("usage: %s <pin 0 - 7>\n",argv[0]);
    exit(1);
  }
  
  pin = atoi(argv[1]);
  if ((pin < 0) || (pin > 7)){
    printf("invalid pin number - must be in range 0 to 7\n");
    exit(1);
  }

  printf ("Raspberry Pi blinking LED on pin %d\n", pin) ;

  initLED();
  
  for(i=0;i<10;i++)
  {
    turnOnLED();
    delay(500);
    turnOffLED();
    delay(500);
  }
}
