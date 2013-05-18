/*
 * gpio_driver.h
 *
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
 * Ceeb
 * (C) 2013 Fatkahawai
 */


// Interface
int ledSetup();
void ledOn();
void ledOff();

