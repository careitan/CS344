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
void ProcessEXIT(int ShellProcs[]);
int ProcessCD();
void ProcessSTATUS();

// Utility Functions for executing the program
bool IsValidCommandLine(char* string);
void ParseCommandline(char* arguments[], char* string);
void SpawnFork(pid_t ProcessID);
void SpawnExec(pid_t ProcessID);
void SpawnWaitPid(pid_t ProcessID);
int getlineClean(char *line, int max);
// void AddProcessToStack(struct pthread_t* v[], struct pthread_t newProc);
void stripLeadingAndTrailingSpaces(char* string);

// Supporting Globals to Handle Key resources throughout program.
char* CurrentStatus;			// Holding string for the message of the current status to feed to GetStatus process.
bool IsExit;
int MainPID;					// Holding variable for the Main Process ID to be passed to child process as needed to support program requirements.

int main(int argc, char* argv[])
{
	IsExit = false;
	int ReturnVal=-1;
	char commandLine[MAXLINE_LENGTH];  // read in the stdin put.
	char* ARGS;
	int ShellBgProcs[1];		// Array to hold the Process Threads running.

	// TODO: Are we going to need a MUTEX here to handle process and thread locks.
	MainPID = getpid();
	ShellBgProcs[0] = 0;  	// initialize the memory space for the start of the program.

	// DEBUG
	// printf("Process Thread for smallsh: %i\n", getpid());

	// Main loop to process user input

	while(IsExit==false){
		printf("\n: ");
		memset(commandLine, '\0', MAXLINE_LENGTH);
		ReturnVal = getlineClean(commandLine, MAXLINE_LENGTH);

		if (ReturnVal > 0){
			// Got something passed into the shell program via stdin.
			// Check for validity before processing it.
			if (IsValidCommandLine(commandLine))
			{
				// Handle the commandLine that passed into the shell.
				// Parse the commandLine into the necessary shell operations.
				ParseCommandline(&ARGS, commandLine);




				printf("%s", commandLine);
			}
			// Handle the line if something odd appeared.
			else{
				// printf("INVALID: %s", commandLine);
			}
		}

		if (strcmp(commandLine,"exit") == 0) ProcessEXIT(ShellBgProcs);
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
void ProcessEXIT(int ShellProcs[])
{
	// TODO: Shut down all of the background child processes.
	for (int i = 0; i < sizeof(ShellProcs)/sizeof(int); ++i)
	{
		printf("%i", ShellProcs[i]);
	}
	
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
bool IsValidCommandLine(char* string)
{
	bool ReturnVal=true;
	int Length = strlen(string);

	if (Length > MAXLINE_LENGTH) ReturnVal = false;	// Exceeded Maximum Line Length.
	if (strcspn(string,"(") < Length || strcspn(string,")") < Length) ReturnVal = false;	// Contains the use of Parens like a Subshell.
	if (strcspn(string,"&") < Length-1 && strcmp(string,"&")==0) ReturnVal = false; // Use of the '&' somewhere other than in the trailing position of function.										
	if (strcspn(string,"#")==0) ReturnVal = false;	// Comment String nothing to see here move along.

	return ReturnVal;
}

// create an Arguements array to pass into the Exec() Command.
// split out the string into Arguements and then append the NULL to the last element in the array.
// Based on String to Array parsing routine found online at: https://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings
void ParseCommandline(char* arguments[], char* string)
{
	assert(string!=NULL);
	int n_spaces=0;
	char *p = strtok(string, " ");
	char *TempString = '\0';

	arguments = malloc(sizeof(char*) * strlen(string));

	while(p != NULL)
	{
		if (arguments == NULL)
			exit(-1);	/* memory allocation failed */

		if ((n_spaces) >= 1) // check to see if we are in the second arguement and look for ECHO on Arg[0]
		{
			if (strcmp(arguments[0],"echo")==0)
			{
				scanf(TempString, "%s%s%s", arguments[n_spaces], " ", p); 

				arguments[n_spaces]=TempString;
			}
			else
			{
				scanf(arguments[n_spaces], "%s", p);
			}
		}
		else
		{
			arguments[n_spaces] = p;
		}

		p = strtok(NULL, " ");
		n_spaces++;
	}

	/* realloc one extra element for the last NULL */
	arguments = realloc(arguments, (sizeof(char*) * strlen(string)) +1);
	arguments[n_spaces] = 0;

	// DEBUG
	for (int i = 0; i < n_spaces; ++i) printf("%s",arguments[i]);	

	return;
}

// function to spawn a seperate thread via a fork.
void SpawnFork(pid_t ProcessID)
{

}

// function to spawn the Exec function.
void SpawnExec(pid_t ProcessID)
{

}

// function to hold for a thread via waitpid.
void SpawnWaitPid(pid_t ProcessID)
{

}

/*
void AddProcessToStack(struct pthread_t* v[], struct pthread_t newProc)
{
	assert(v!=NULL);

	for (int i = 0; i < sizeof(v)/sizeof(struct pthread_t); ++i)
	{
		if (v[i]!=NULL)
		{
			
		}
	}
}
*/

// https://stackoverflow.com/questions/352055/best-algorithm-to-strip-leading-and-trailing-spaces-in-c
void stripLeadingAndTrailingSpaces(char* string)
{
     assert(string);

     /* First remove leading spaces */
     const char* firstNonSpace = string;

     while(*firstNonSpace != '\0' && isspace(*firstNonSpace))
     {
          ++firstNonSpace;
     }

     size_t len = strlen(firstNonSpace)+1;         
     memmove(string, firstNonSpace, len);

     /* Now remove trailing spaces */
     char* endOfString = string + len;

     while(string < endOfString  && isspace(*endOfString))
     {
          --endOfString ;
     }
     *endOfString = '\0';
}