README

RPI_NFC

an example of a TCP client that connects to a remote server, then polls a NFC card for transactions and sends these records to the server as TCP messages, and waits for an ACK from the server

to compile:
 >  gcc client.c - o client

 to start, open terminal window and run client with the hostname and ort number as argument
 > client localhost 3000

this starts the client which connects to port 3000 on localhost

Copyright (c) 2013 Pink Pelican NZ Ltd <bob@pink-pelican.com>

