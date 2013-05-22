/*
 * @file led_driver.h
 * @brief public interface for led_driver.c
 *
 * @author Robert Drummond
 * Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>
 */

#include <stdbool.h>

// Public Interface
// function prototypes 

int  initLED( void );
void turnOnLED( void );
void turnOffLED( void );
bool isLEDon( void );

