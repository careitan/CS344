/****************************************
*	CS 344 - Program 4 Encoding Daemon
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: Hosting Daemon for the Encrypting functions of Program 4
*
*****************************************/
#define _GNU_SOURCE

#define NET_READ_BUFFER 2048
#define MAX_FILE_NAME 256

typedef int bool;
#define true  0
#define false 1

#include "Program4_lib.h"
#include "dynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

DynArr *Processes;

int main(int argc, char* argv[])
{
	// Initialize variables for use in the program.
	Processes = createDynArr(2);
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[NET_READ_BUFFER+1];
	struct sockaddr_in serverAddress, clientAddress;
	/*
	int pid;
	*/

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	// Do a while loop here to establish a server.
	while(1){
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		if (sizeDynArr(Processes) < 6)
		{
			establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
			if (establishedConnectionFD < 0) error("ERROR on accept");
			addDynArr(Processes, establishedConnectionFD);
			// TODO: Fork at this point and use a child process to handle the process of the file stream.

			// setup to file descriptors for the files that we are going to write on the server side.
			/*
			int SourceFile; KeyFile;
			char SourceFile[MAX_FILE_NAME];
			char KeyFile[MAX_FILE_NAME];
			SourceFile=-5; KeyFile=-5;
*/

			// Get the message from the client and display it
			memset(buffer, '\0', NET_READ_BUFFER+1);

			// while(strcmp(buffer, "@@TERM@@") != 0){
			while((charsRead = recv(establishedConnectionFD, buffer, NET_READ_BUFFER, 0)) > 0){
				// Read the client's message from the socket
				// DEBUG
				printf("SERVER: Current Value of charsRead is: %i\n",charsRead);
				printf("SERVER: I received this from the client: \"%s\"\n", buffer);
				
				if (strcmp(buffer, "@@TERM@@") == 0){ charsRead = 0; break; }

				// Setup for next iteration of loop
				memset(buffer, '\0', NET_READ_BUFFER+1);
			}

			if (charsRead < 0) error("ERROR reading from socket");

			// TODO: Create the Encrypted message text here.

			// Send a Success message back to the client
			// TODO: Replace this with the send return response back to client.
			// TODO: Use this method to send back down the encrypted file that was created.
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");

			// Close out the connection.
			close(establishedConnectionFD); // Close the existing socket which is connected to the client

			removeDynArr(Processes, establishedConnectionFD);
			// TODO: End the fork process here for the client connect.

		}else{
			printf("SERVER: Unable to establish a new connection at this time.\n");
		}
	}
	
	// TODO: Move this step up to the parent process.
	close(listenSocketFD); // Close the listening socket
	
	return 0;
}