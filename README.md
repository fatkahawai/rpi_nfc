README

RPI_NFC
=======

A Raspberry Pi application:

A TCP client that connects to a remote server, then polls a PN532 NFC device for NFC transactions. When it detects a NFC target, it blinks the LED via a GPIO output, and sends the NFC transaction record to the server as a JSON-encoded TCP message, and waits for an ACK from the server.
NFC transactions are not stored locally. 

Modules:
- nfc_driver.c
- led_driver.c
- tcp_client.c

Libraries used
- libnfc

Testing
=======
each module has a unit test program, which is compiled with the module and runs standalone to test the module's functions
- nfc_driver_test.c
- led_driver_test.c
- tcp_client_test.c

To compile
==========
 >  ./compile_rpi_nfc.sh

 to start, run the client with the hostname and port number as argument, e.g.
 > rpi_nfc 192.168.0.200 51717

this starts the client which connects to port 51717 on 192.168.0.200, opens the NFC device

To dos
======
1. implement store and forward, so that NFC transactions aren't lost if the TCP connection is down


Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>

