# Client-Server Chat Application with Pthreads and Select

This is a simple chat application built using C programming language that allows multiple clients to connect to a server and communicate with each other via text messages. This program uses POSIX threads and the select() function to handle multiple client connections and incoming messages.

# Server
The server listens on a specific port and accepts incoming connections from clients. For each new client, a new thread is created using pthread_create(), which calls the handle_client() function to handle incoming messages.\

In the handle_client() function, the incoming message is received using recv() function, and the server responds with a generic message using send() function.

# Client
The client connects to the server using the IP address and port number of the server. Once connected, the user is prompted to enter a username, which is sent to the server as a login command. The user can then send chat messages to other users by entering text in the terminal. Private messages can also be sent by prefixing the username with @.\

The select() function is used to wait for incoming messages from the server or user input from the terminal. The file descriptor set is initialized to include the standard input file descriptor (STDIN_FILENO) and the socket file descriptor (sock). The select() function blocks until there is data available on one of these file descriptors.\

How pthreads and select() functions are used\
The pthread_create() function is used to create a new thread for each client connection. This allows multiple clients to connect to the server concurrently and communicate with each other without blocking each other.\

The select() function is used to monitor multiple file descriptors for data readiness. In this program, it is used to wait for incoming messages from the server or user input from the terminal. When data is available on one of these file descriptors, the select() function returns, and the program can read or write data on that file descriptor. Using select() instead of blocking I/O operations allows the program to be more responsive and handle multiple connections efficiently.

# Usage
$ make\
To run the server, compile the server.c file and run the executable:\
$ ./server

To run the client, compile the client.c file and run the executable:
$ ./client

# After connecting with multiple clients by running the executable file
use the following commands\
Enter command: login <username>
Enter command: chat [@ username] <message>   -  This command sends the message to specified user. When the @ is not in the command the messsage is
                                                broadcasted to all the other userss onlne.\
Enter command: logout\
Enter command: exit                          -  Returns to the terminal.\
