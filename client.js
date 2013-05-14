#!/usr/bin/env node
var WebSocketClient = require('websocket').client;

var client = new WebSocketClient();

client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {
    console.log('WebSocket client connected');
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('echo-protocol Connection Closed');
    });
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log("Received: '" + message.utf8Data + "'");
        }
    });

    function sendMessage(msg) {
        if (connection.connected) {

            connection.sendUTF(msg);
            setTimeout(sendMessage, 1000);
        }
    }
    
    sendMessage("hello world");
    sendMessage("second message");
});

client.connect('ws://localhost:51717/', 'echo-protocol');