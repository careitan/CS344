/*******************************
* OSU CS344 - Fall Quarter 2017
* Program 3 - Small Shell Project
* (Craig) Allan Reitan
* 10-28-2017
* 
* Assignment is to create a shell engine in C
*
********************************/
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int bool;
#define true  1
#define false 0

// Program defined requirements
#define MAXLINE_LENGTH 	2048
#define MAXARGS			512

// Program required Built-In Functions
void ProcessEXIT();
int ProcessCD();
void ProcessSTATUS();

// Utility Functions for executing the program
bool ParseValidCommandLine(char* string);
void SpawnFork(pid_t ProcessID);
void SpawnExec(pid_t ProcessID);
void SpawnWaitPid(pid_t ProcessID);
int getlineClean(char *line, int max);

// Supporting Globals to Handle Key resources throughout program.
char* CurrentStatus;
bool IsExit;
struct pthread_t *ShellBgProcs;			// Array to hold the Process Threads running.

int main(int argc, char* argv[])
{
	IsExit = false;
	int ReturnVal=-1;
	char commandLine[MAXLINE_LENGTH];  // read in the stdin put.

	// DEBUG
	// printf("Process Thread for smallsh: %i\n", getpid());

	// Main loop to process user input

	while(IsExit==false){
		printf("\n: ");
		memset(commandLine,'\0',MAXLINE_LENGTH);
		ReturnVal = getlineClean(commandLine, MAXLINE_LENGTH);

		if (ReturnVal > 0){
			// Got something passed into the shell program via stdin.
			// Check for validity before processing it.
			if (ParseValidCommandLine(commandLine))
			{
				printf("%s", commandLine);
			}
			// Handle the line if something odd appeared.
			else{
				// printf("INVALID: %s", commandLine);
			}
		}

		if (strcmp(commandLine,"exit") == 0) ProcessEXIT();
	};

	printf("\nHello World! Away we go...\n");
	return 0;
};

// K&R C Programming Language, 2nd Ed.  Page 165; Refactored for this program.
// Reads a line and returns length.
int getlineClean(char *line, int max)
{
	// TODO: Implement a feature to prompt the user for input if STDIN is already empty.
	if (fgets(line, max, stdin)==NULL)
		return 0;
	else{
		line[strcspn(line,"\n")]='\0';
		return strlen(line);
	}
}

// Function implementations for the built-in functions.
void ProcessEXIT()
{
	// TODO: Shut down all of the background child processes.
	
	IsExit = true;
	printf("\nValue of IsExit is: %i\n", IsExit);
}
int ProcessCD()
{

	return 0;
}
void ProcessSTATUS()
{
	printf("%s\n", CurrentStatus);
}

// function to parse out the commandLine for validity
bool ParseValidCommandLine(char* string)
{
	bool ReturnVal=true;
	int Length = strlen(string);

	if (Length > MAXLINE_LENGTH) ReturnVal = false;	// Exceeded Maximum Line Length.
	if (strcspn(string,"(") < Length || strcspn(string,")") < Length) ReturnVal = false;	// Contains the use of Parens like a Subshell.
	if (strcspn(string,"&") < Length-1 && strcmp(string,"&")==0) ReturnVal = false; // Use of the '&' somewhere other than in the trailing position of function.										
	if (strcspn(string,"#")==0) ReturnVal = false;	// Comment String nothing to see here move along.

	return ReturnVal;
}

// function to spawn a seperate thread via a fork.
void SpawnFork(pid_t ProcessID)
{

}

// function to spawn a seperate thread via a fork.
void SpawnExec(pid_t ProcessID)
{

}

// function to spawn a seperate thread via waitpid.
void SpawnWaitPid(pid_t ProcessID)
{

}

