/****************************************
*	CS 344 - Program 4 Decoding algorithms
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: Implementation for the Decrypting functions of Program 4
*
*****************************************/
#define _GNU_SOURCE
#define NET_READ_BUFFER 2048
#define MAX_FILE_NAME 256

typedef int bool;
#define true  0
#define false 1

#include "Program4_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
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
	char filebuffer[MAX_FILE_NAME];
	bool IsFilesValid = false;
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s inputfile keyfile port\n", argv[0]); exit(0); } // Check usage & args

	IsFilesValid = IsValidFileSet(argv[1], argv[2]);
	if (IsFilesValid != 0) { fprintf(stderr, "otp_dec error: input contains bad characters\n"); exit(0); }

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
	if (socketFD < 0) { error("ERROR creating socket on port provided."); exit(2); }
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
		error("CLIENT: ERROR connecting to server on port provided.");
		exit(2);
	}

	// Send up the Sentinel to the server for the purpose of identifying the client.
	charsWritten = send(socketFD, "@@otp_dec@@", strlen("@@otp_dec@@"), 0);
	if (charsWritten < 0){
		fprintf(stderr, "Error: could not contact otp_dec_d on port %i\n", portNumber);
		exit(1);
	}
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data (File Names) written to socket!\n");

	// Gap and wait for the send buffer to clear so that we know all data got up to the server before proceeding.
	int checkSend = -5;  // Bytes remaining in send buffer
	do
	{
	  ioctl(socketFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
	  //printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are:
	}
	while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
	if (checkSend < 0)  // Check if we actually stopped the loop because of an error
	  error("ioctl error");

	// Pause for one second just open up the gap between the empty buffer and next step.
	sleep(1);

	// loop through the two files to upload.
	for (int i = 1; i < 3; ++i)
	{			
		// Send up the name of the file as starting block of the stream.
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
		memset(filebuffer, '\0', sizeof(filebuffer)); // Clear out the buffer array
		//strcpy(filebuffer, "##");
		strcat(filebuffer, argv[i]);
		strcat(filebuffer, "_srv##");
		strcpy(buffer, filebuffer);
		charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data (File Names) written to socket!\n");
		
		// Gap to wait for that header block to be sent up.
		checkSend = -5;  // Bytes remaining in send buffer
		do
		{
		  ioctl(socketFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
		  //printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are:
		}
		while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
		if (checkSend < 0)  // Check if we actually stopped the loop because of an error
		  error("ioctl error");

		// Prepare file to be sent up as the stream
		int FileUpload = GetFileDescriptorRead(argv[i]);
		if (FileUpload > 0)
		{
			memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

			while (read(FileUpload, buffer, NET_READ_BUFFER) != 0){
				// buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that maybe in the file.
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

		// Pause long enough to let the server process the last block that was sent up.
		sleep(1);
	}
	
	// Send Termination String to the Server
	sleep(1);
	send(socketFD, "@@TERM@@", strlen("@@TERM@@"), 0);

	// Gap and wait for the send buffer to clear so that we know all data got up to the server before proceeding.
	checkSend = -5;  // Bytes remaining in send buffer
	do
	{
	  ioctl(socketFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
	  //printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are:
	}
	while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
	if (checkSend < 0)  // Check if we actually stopped the loop because of an error
	  error("ioctl error");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

	// Read data from the socket, leaving \0 at end
	while((charsRead = recv(socketFD, buffer, NET_READ_BUFFER, 0)) > 0){

		// push buffer to stdout for writting to output file.
		fprintf(stdout, "%s", buffer);
		fflush(stdout);
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	}
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	close(socketFD); // Close the socket

	return 0;
}