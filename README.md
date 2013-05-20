README

RPI_NFC

an example of a TCP client that connects to a remote server, then polls a NFC card for transactions. when it detects a NFC swipe, it blinks the LED via a GPIO output, and sends the transaction data record to the server as a JSON-encoded TCP message, and waits for an ACK from the server.

Modules:
- nfc_driver.c
- led_driver.c
- tcp_client.c

Libraries used
- libnfc

Testing:
each module has a unit test program, which is compiled with the module and runs standalone to test the module's functions
- nfc_driver_test.c
- led_driver_test.c
- tcp_client_test.c

to compile:
 >  ./compile_rpi_nfc.sh

 to start, run the client with the hostname and port number as argument, e.g.
 > rpi_nfc 192.168.0.200 51717

this starts the client which connects to port 51717 on 192.168.0.200, opens the NFC device

Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>

