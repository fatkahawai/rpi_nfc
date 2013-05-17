README

NFC_SERVER and RPI_CLIENT

an example of an app server in Node & Express that provides: 

1. a socket server
- accepts TCP connections from a client implemented on the Raspberry Pi
- accepts messages from the client which contain NFC transactions and status events 

2. a web server and RESTful API which:
- serves a webapp to web clients to monitor NFC activity 
- accepts Ajax reqeusts from that webapp to display NFC transaction data. API calls respond to a request by returnig a JSON object which can be directly referenced.

to start, open terminal window and run node with the myapi.js file as argument
    > node myapi.js

this starts the http server using port 3000

When a user now browses to http://<hostname>:3000, index.html will be loaded to the browser (served by the express webserver implemented in myapi.js) 

