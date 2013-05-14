This is a simple tutorial on using sockets for interprocess communication.


The client server model

Most interprocess communication uses the client server model. These terms refer to the two processes which will be communicating with each other. One of the two processes, the client, connects to the other process, the server, typically to make a request for information. A good analogy is a person who makes a phone call to another person.
Notice that the client needs to know of the existence of and the address of the server, but the server does not need to know the address of (or even the existence of) the client prior to the connection being established.

Notice also that once a connection is established, both sides can send and receive information.

The system calls for establishing a connection are somewhat different for the client and the server, but both involve the basic construct of a socket.
A socket is one end of an interprocess communication channel. The two processes 
each establish their own socket.

The steps involved in establishing a socket on the client side are as follows:

Create a socket with the socket() system call
Connect the socket to the address of the server using the connect() system call
Send and receive data. There are a number of ways to do this, but the simplest is to use the read() and write() system calls.
The steps involved in establishing a socket on the server side are as follows:

Create a socket with the socket() system call
Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists of a port number on the host machine.
Listen for connections with the listen() system call
Accept a connection with the accept() system call. This call typically blocks until a client connects with the server.
Send and receive data


Socket Types

When a socket is created, the program has to specify the address domain and the socket type. Two processes can communicate with each other only if their sockets are of the same type and in the same domain.

There are two widely used address domains, the unix domain, in which two processes which share a common file system communicate, and the Internet domain, in which two processes running on any two hosts on the Internet communicate. Each of these has its own address format.

The address of a socket in the Unix domain is a character string which is basically an entry in the file system.

The address of a socket in the Internet domain consists of the Internet address of the host machine (every computer on the Internet has a unique 32 bit address, often referred to as its IP address).
In addition, each socket needs a port number on that host.
Port numbers are 16 bit unsigned integers. 
The lower numbers are reserved in Unix for standard services. For example, the port number for the FTP server is 21. It is important that standard services be at the same port on all computers so that clients will know their addresses. 
However, port numbers above 2000 are generally available.

There are two widely used socket types, stream sockets, and datagram sockets. Stream sockets treat communications as a continuous stream of characters, while datagram sockets have to read entire messages at once. Each uses its own communciations protocol.

Stream sockets use TCP (Transmission Control Protocol), which is a reliable, stream oriented protocol, and datagram sockets use UDP (Unix Datagram Protocol), which is unreliable and message oriented.

The examples in this tutorial will use sockets in the Internet domain using the TCP protocol.



Sample code
compile and run the two programs to see what they do.

server.c
client.c

compile them separately into two executables called server and client.

They probably won't require any special compiling flags, but on some solaris systems you may need to link to the socket library by appending -lsocket to your compile command.

Ideally, you should run the client and the server on separate hosts on the Internet. Start the server first. Suppose the server is running on a machine called cheerios. When you run the server, you need to pass the port number in as an argument. You can choose any number between 2000 and 65535. If this port is already in use on that machine, the server will tell you this and exit. If this happens, just choose another port and try again. If the port is available, the server will block until it receives a connection from the client. Don't be alarmed if the server doesn't do anything;

It's not supposed to do anything until a connection is made.


Here is a typical command line:

server 51717
To run the client you need to pass in two arguments, the name of the host on which the server is running and the port number on which the server is listening for connections.

Here is the command line to connect to the server described above:

client cheerios 51717

The client will prompt you to enter a message. 
If everything works correctly, the server will display your message on stdout, send an acknowledgement message to the client and terminate. 
The client will print the acknowledgement message from the server and then terminate.
You can simulate this on a single machine by running the server in one window and the client in another. In this case, you can use the keyword localhost as the first argument to the client.