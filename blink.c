/*
 * blink.c:
 *      blinks the first LED
 *      Gordon Henderson, projects@drogon.net
 *
 RPi    Physical  wiringPi
 Name    Pin No.   Pin No.
  0       11         0        
  1       12         1
  2       13         2
  3       15         3
  4       16         4
  5       18         5
  6       22         6
  7       07         7

 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

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

  if (wiringPiSetup () == -1)
    return 1 ;

  pinMode (pin, OUTPUT) ;         // aka BCM_GPIO pin 17

  for (;;)
  {
    digitalWrite (pin, 1) ;       // On
    delay (500) ;               // mS
    digitalWrite (pin, 0) ;       // Off
    delay (500) ;
  }
  return 0 ;
}
