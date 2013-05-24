#!/bin/bash

echo gcc -o led_driver_test led_driver_test.c led_driver.c ../wiringPi/wiringPi/libwiringPi.so.2.0

gcc -o led_driver_test led_driver_test.c led_driver.c ../wiringPi/wiringPi/libwiringPi.so.2.0
