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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>

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
			
			int SourceFP, KeyFP, CurrentFP;
			char SourceFile[MAX_FILE_NAME];
			char KeyFile[MAX_FILE_NAME];
			char ResultFile[32];
			SourceFP=-5; KeyFP=-5;
			
			bool IsTerminated = false;
			bool IsFilesValid = false;

			memset(SourceFile, '\0', MAX_FILE_NAME);
			memset(KeyFile, '\0', MAX_FILE_NAME);

			while(IsTerminated !=0)
			{
				// Peek ahead to see what type of message is coming in read
				memset(buffer, '\0', NET_READ_BUFFER+1);
				recv(establishedConnectionFD, buffer, MAX_FILE_NAME, MSG_PEEK);
				if (strstr(buffer, "##") != NULL)
				{
					// Have a file name sent up for processing.
					char* p = strtok(buffer, "#");
					if(strlen(SourceFile)==0)
					{
						strcpy(SourceFile,p);
					}else
					{
						strcpy(KeyFile,p);
					}

				}else if (strstr(buffer, "@@TERM@@") != NULL)
				{ 
					IsTerminated = true; 
				}

				// Read the client's message from the socket
				memset(buffer, '\0', NET_READ_BUFFER+1);
				if ((charsRead = recv(establishedConnectionFD, buffer, NET_READ_BUFFER, 0)) > 0)
				{
					if (strstr(buffer, "##") != NULL && SourceFP==-5)
					{
						// First one up is the SourceFile.
						SourceFP=open(SourceFile, O_RDWR | O_CREAT | O_TRUNC, 0664);
						if (SourceFP == -1)
						{
							fprintf(stderr, "SERVER: Unable to open the Source File for writing.\n");
						}else
						{
							CurrentFP = SourceFP;
						}
					}else if (strstr(buffer, "##") != NULL)
					{
						// Second one up is the Key File.
						KeyFP=open(KeyFile, O_RDWR | O_CREAT | O_TRUNC, 0664);
						if (KeyFP == -1)
						{
							fprintf(stderr, "SERVER: Unable to open the Key File for writing.\n");
						}else
						{
							CurrentFP = KeyFP;
						}
					}else if (strstr(buffer, "@@TERM@@") != NULL)
					{ 
						// Set the termination flag for the While Loop.
						IsTerminated = true; 
					}else
					{
						// write bytestream to the current file pointer.
						write(CurrentFP, buffer, strlen(buffer));
					}
				} // end if block for the RECV read and file write.

				if (charsRead < 0)
				{
					error("ERROR reading from socket");
				}

				// Setup for next iteration of loop
				memset(buffer, '\0', NET_READ_BUFFER+1);
				
				// DEBUG
				// printf("SERVER: At End of While Loop, IsTerminated = %i\n", IsTerminated);
			} // end of while loop.

			// Reset the file pointer to the beginning of the file.
			lseek(SourceFP, 0, SEEK_SET);
			lseek(KeyFP, 0, SEEK_SET);

			// DEBUG
			// printf("SERVER: The Current values of SourceFile, KeyFile : %s, %s ;\n", SourceFile, KeyFile);

			IsFilesValid = IsValidFileSet(SourceFile, KeyFile);
			if (IsFilesValid != 0) 
			{ 
				fprintf(stderr, "SERVER: ERROR Detected, Input Files are invalid.\n"); 
				close(establishedConnectionFD); // Close the existing socket which is connected to the client

				removeDynArr(Processes, establishedConnectionFD);
				exit(0); 
			}

			// Setup for Creating the Encrypted File.

			// Reset the file pointer to the beginning of the file.
			lseek(SourceFP, 0, SEEK_SET);
			lseek(KeyFP, 0, SEEK_SET);

			// TODO: Create the Encrypted message text here.
			memset(ResultFile, '\0', 32);
			strcpy(ResultFile, "Results");
			int pidResult=0;
			pidResult = getpid();
			strcat(ResultFile, integer_to_string(pidResult));
			
			// DEBUG
			// printf("SERVER: The Current Value of ResultFile is : %s \n", ResultFile);

			int ResultFP = open(ResultFile, O_RDWR | O_CREAT | O_TRUNC, 0664);
			if (ResultFP == -1)
			{
				fprintf(stderr, "SERVER: Unable to Open the Encrypt Results file: %s\n", ResultFile);
			}

			// process the files creating the encrypted file.
			int FileEncrypted = EncryptData(SourceFP, KeyFP, ResultFP);

			if (FileEncrypted <= 0)
			{
				fprintf(stderr, "SERVER: Unable to Encrypt Results file, FileEncrypted = %i\n", FileEncrypted);
			}

			// Send a Success message back to the client
			lseek(ResultFP, 0, SEEK_SET);
			memset(buffer, '\0', NET_READ_BUFFER+1);

			// TODO: Use this method to send back down the encrypted file that was created.
			while (read(ResultFP, buffer, NET_READ_BUFFER) != 0){
				charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Send success back
				if (charsRead < 0) error("SERVER: ERROR writing to socket");
				if (charsRead < strlen(buffer)) printf("SERVER: WARNING: Not all data written to socket!\n");
			}

			// Gap and wait for the send buffer to clear so that we know all data got outbound from the server before proceeding.
			int checkSend = -5;  // Bytes remaining in send buffer
			do
			{
			  ioctl(establishedConnectionFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
			  //printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are:
			  sleep(2);
			}
			while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
			if (checkSend < 0)  // Check if we actually stopped the loop because of an error
			  error("SERVER: ioctl error");

			// Close out the connection.
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			removeDynArr(Processes, establishedConnectionFD);

			// Cleanoff the server of the artifacts.
			close(SourceFP);
			close(KeyFP);
			close(ResultFP);

			remove(SourceFile);
			remove(KeyFile);
			remove(ResultFile);
			// TODO: End the fork process here for the client connect.

		}else
		{
			printf("SERVER: Unable to establish a new connection at this time.\n");
		}
	}
	
	// TODO: Move this step up to the parent process.
	close(listenSocketFD); // Close the listening socket
	
	return 0;
}