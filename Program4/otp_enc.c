/****************************************
*	CS 344 - Program 4 Encoding child process executable.
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: Implementation for the Encrypting functions of Program 4
*
*****************************************/
#define _GNU_SOURCE
#define NET_READ_BUFFER 2048

typedef int bool;
#define true  0
#define false 1

#include "Program4_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[NET_READ_BUFFER+1];
	bool IsFilesValid = false;
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s inputfile keyfile port\n", argv[0]); exit(0); } // Check usage & args

	IsFilesValid = IsValidFileSet(argv[1], argv[2]);
	if (IsFilesValid != 0) { fprintf(stderr, "ERROR Detected, File is Larger than Key.\n"); exit(0); }

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// loop through the two files to upload.
	for (int i = 1; i < 3; ++i)
	{	
		// TODO: Replace this with a read from the input files
		int FileUpload = GetFileDescriptorRead(argv[i]);

		if (FileUpload > 0)
		{
			memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

			while (read(FileUpload, buffer, NET_READ_BUFFER) >= 0){
				// Send message to server
				charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
				if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
				if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
				memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
			}

			close(FileUpload);
		}else{
			fprintf(stderr, "Unable to access for upload: %s\n", argv[i]);
		}

	}

	// Get return message from server
	// TODO: Build this up as a while loop.
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

	// Read data from the socket, leaving \0 at end
	while((charsRead = recv(socketFD, buffer, NET_READ_BUFFER, 0)) != EOF){
		
		if (charsRead < 0) error("CLIENT: ERROR reading from socket");

		// push buffer to stdout for writting to output file.
		fprintf(stdout, "%s", buffer);
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = (charsRead == 0)? -1 : charsRead;
	}

	close(socketFD); // Close the socket

	return 0;
}