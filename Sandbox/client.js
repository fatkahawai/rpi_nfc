#!/usr/bin/env node
/*
 * CLIENT.JS
 *
 * open socket to connect with server
 * accept push messages from server. 
 * when terminating the connection, send "BYE" to server.   
 */

var net = require('net');
var timerID;
var portNum;
var hostName;
var requestStr;
var timeoutPeriod = 2000;      // timeout if no message in 2 s
var pollingPeriod = 3000;   // poll every 5s

if( process.argv.length < 4 ){
  console.log("usage: node my_socket.io.js hostName portNum");
  return;
}
hostName = process.argv[2];
portNum = process.argv[3];

console.log('connecting to server on '+hostName+' at '+portNum);

//var client = net.connect({host: 'localhost',port: 51717},
var client = net.connect(portNum, hostName, function() { //'connect' listener
  var i = 0;

  console.log('client connected to server on '+hostName+' at '+client.remoteAddress+':'+client.remotePort );

  // poll server every 2s
  timerID = setInterval( function(){
      console.log('sending polling request '+i);
      requestStr = 'REQUEST'+i;
      client.write(requestStr);
      client.setTimeout(timeoutPeriod);
      i++;
      console.log('sent request ['+requestStr+']');
  }, pollingPeriod );
});

// display messages received from the server
client.on('data', function(data) {
  client.setTimeout(0);

  console.log('Message received ('+data.length+' bytes): '+data.toString()+'\n');

  if( data == 'ACK'){
  	console.log("server acknowledged polling message (ACK received)");
  }
  else if( data == 'BYE'){
    console.log("server closing socket");
    client.end('BYE');
  }
  //client.end(); // if you call this, it sends some bytes to the server, which thinks it s a new message
});

// on 
client.on('timeout', function() {
  console.log('TIMEOUT: timeout waiting for a response to polling message. continuing to wait.');
});

// when server shuts down socket, disconnect client
client.on('error', function(err) {
  console.log('ERROR: '+err);
  client.setTimeout(0);
  clearInterval( timerID );
  client.destroy();
});

// when server shuts down socket, disconnect client
client.on('end', function() {
  console.log('END: Disconnecting');
  client.end('BYE');
  client.setTimeout(0);
  clearInterval( timerID );
});

// when server shuts down socket, disconnect client
client.on('close', function() {
  console.log('CLOSE: Client disconnected. Socket closed.');
  client.setTimeout(0);
  clearInterval( timerID );
});


