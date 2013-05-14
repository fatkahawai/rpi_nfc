#!/usr/bin/env node
var socket = require('socket.io-client')('http://localhost:51717');

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

