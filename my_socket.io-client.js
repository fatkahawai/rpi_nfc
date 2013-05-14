#!/usr/bin/env node


var net = require('net');
var timerID;
var portNum;
var hostName;
var number_of_requests;
var requestStr;

if( process.argv.length < 3 ){
  console.log("usage: node my_socket.io.js hostName portNum number_of_requests");
  return;
}
hostName = process.argv[2];
portNum = process.argv[3];
number_of_requests = process.argv[4];

//var client = net.connect({host: 'localhost',port: 51717},
var client = net.connect(portNum, hostName, function() { //'connect' listener
  var i = 0;

  console.log('client connected to server on '+hostName+' at '+client.remoteAddress+':'+client.remotePort );

  // poll server every 2s
  timerID = setInterval( function(){
    if( i >= number_of_requests ){
      console.log("sending [CLOSING]");
      client.end('CLOSING');
    } else {
      console.log('sending polling request '+i);
      requestStr = 'REQUEST'+i;
      client.write(requestStr);
      client.setTimeout(1000);
      i++;
      console.log('sent request ['+requestStr+']');
    }
  }, 2000 );
});

// display messages received from the server
client.on('data', function(data) {
  client.setTimeout(0);

  console.log('Response received ('+data.length+' bytes): '+data.toString()+'\n');

  if( data == 'CLOSE'){
  	console.log("server closing socket");
    client.end('CLOSE');
  }
  //client.end(); // if you call this, it sends some bytes to the server, which thinks it s a new message
});

// on 
client.on('timeout', function() {
  console.log('timeout waiting for response');
});

// when server shuts down socket, disconnect client
client.on('error', function() {
  console.log('ERROR');
  client.setTimeout(0);
  clearInterval( timerID );
  socket.destroy();
});

// when server shuts down socket, disconnect client
client.on('end', function() {
  console.log('Disconnecting');
  client.end('CLOSE');
  client.setTimeout(0);
  clearInterval( timerID );
});

// when server shuts down socket, disconnect client
client.on('close', function() {
  console.log('Client disconnected. Socket closed.');
  client.setTimeout(0);
  clearInterval( timerID );
});

/* datagram sockets - doesnt regaister at server end
var dgram = require('dgram');

var message = new Buffer("Some bytes");
var client = dgram.createSocket("udp6");

client.send(message, 0, message.length, 51717, "localhost", function(err, bytes) {
  client.close();
});

/* doesnt work until version 1.0 of socket.io
var socket = require('socket.io')('http://localhost:51717');

socket.on('connect', function(){
    console.log("connected");

    socket.on('event', function(data){
      console.log(data);
      socket.emit('my event', { my: 'data' });
    });

    socket.on('disconnect', function(){
      console.log("disconnected");        
    });
});

*/