/*
 * @file led_driver.c:
 * @brief control an LED driven by a pin of the RPi GPIO
 *
 * uses the wiringPi RPi GPIO driver library 
 *
 * Mapping the physical pin numbers on the RPi header to the pin names 
 * used by the wiring Pi library:
===========================
 RPi    Physical  wiringPi
 Name    Pin No.   Pin No.
---------------------------
  0       11         0        
  1       12         1
  2       13         2
  3       15         3
  4       16         4
  5       18         5
  6       22         6
  7       07         7
===========================
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "led_driver.h"


#define LED_PIN 4

/*
 * initialise port for output
 *
 * returns: 0 if OK, -1 if failed
 */
int ledSetup(){
  if (wiringPiSetup () == -1)
    return( -1 );

  pinMode (LED_PIN, OUTPUT) ;         // aka BCM_GPIO pin 17
  return( 0 ) ;
}

/*
 * switch ON the LED
 */
void ledOn(){
    digitalWrite (LED_PIN, 1) ;       // ON
}

/*
 * switch OFF the LED
 */
void ledOff(){
    digitalWrite (LED_PIN, 0) ;       // OFF
}


/*
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

  ledSetup();
  
  for(;;)
  {
    ledOn();
    delay(500);
    ledOff();
    delay(500);
  }
}
*/