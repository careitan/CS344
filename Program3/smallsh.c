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
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef int bool;
#define true  1
#define false 0

// Program defined requirements
#define MAXLINE_LENGTH 	2048
#define MAXARGS			512

// Program required Built-In Functions
void ProcessEXIT(int ShellProcs[]);
int ProcessCD(char* arguments[]);
void ProcessSTATUS();

// Utility Functions for executing the program
bool IsValidCommandLine(char* string);
void ParseCommandline(char* arguments[], char* string);
bool IsCommandLineInteral(char* string);
void SpawnFork(char* arguments[]);
void SpawnForkAndExec(char* arguments[]);
void SpawnExec(char* arguments[]);
void SpawnWaitPid(pid_t ProcessID);
int getlineClean(char *line, int max);
void replaceProcessID(char *line, int ProcNum);
void stripLeadingAndTrailingSpaces(char* string);
char* integer_to_string(int x);

// Supporting Globals to Handle Key resources throughout program.
char* CurrentStatus;			// Holding string for the message of the current status to feed to GetStatus process.
bool IsExit;
int MainPID;					// Holding variable for the Main Process ID to be passed to child process as needed to support program requirements.

int main(int argc, char* argv[])
{
	IsExit = false;
	int ReturnVal=-1;
	char commandLine[MAXLINE_LENGTH];  // read in the stdin put.
	char* ARGS[516];
	int ShellBgProcs[256];		// Array to hold the Process Threads running.
	pid_t ChildPid = -5;
	int childExitStatus = -5;

	// TODO: Are we going to need a MUTEX here to handle process and thread locks.
	MainPID = getpid();
	ShellBgProcs[0] = 0;  	// initialize the memory space for the start of the program.

	// DEBUG
	// printf("Process Thread for smallsh: %i\n", getpid());

	// Main loop to process user input

	while(IsExit==false){
		memset(commandLine, '\0', MAXLINE_LENGTH);
		ReturnVal = getlineClean(commandLine, MAXLINE_LENGTH);

		// Parse and replace the $$ indicator for the current process.
		if (strstr(commandLine, "$$") != NULL)
		{
			replaceProcessID(commandLine, MainPID);
		}

		if (ReturnVal > 0){
			// Got something passed into the shell program via stdin.
			// Check for validity before processing it.
			if (IsValidCommandLine(commandLine))
			{
				// Handle the commandLine that passed into the shell.
				// Parse the commandLine into the necessary shell operations.
				ParseCommandline(ARGS, commandLine);

				// TODO: Parse the ARGS to check and see if we have one of the three that we are supposed to handle internaly.
				if (IsCommandLineInteral(ARGS[0]))
				{
					// Found a function is required to be interanlly implemented.
					if (strcmp(ARGS[0], "status")==0)
					{
						ProcessSTATUS();
					}else if (strcmp(ARGS[0], "cd")==0)
					{
						ProcessCD(ARGS);
					}else if (strcmp(ARGS[0], "exit")==0)
					{
						ProcessEXIT(ShellBgProcs);
					}else
					{
						// TODO: Check to see if this is going to be a background process.

				
						// Fork the new process to run and perform the necesary operation.
						switch(ChildPid = fork()){
							case -1:
								CurrentStatus = '\0';
								sprintf(CurrentStatus, "%s %i", "Fork Errored", -1);
								break;

							case 0:
								// Success start Exec
								SpawnExec(ARGS);
								break;

							default:
								// Possibly add the WaitPID() function here.
								fflush(stdout);
								waitpid(ChildPid, &childExitStatus, 0);
								break;
						}
					}
				}

			}
			// Handle the line if something odd appeared.
			else{
				// printf("INVALID: %s", commandLine);
			}
		}

	};

	return 0;
};

// K&R C Programming Language, 2nd Ed.  Page 165; Refactored for this program.
// Reads a line and returns length.
int getlineClean(char *line, int max)
{
	printf("\n: ");

	if (fgets(line, max, stdin)==NULL)
		return 0;
	else{
		line[strcspn(line,"\n")]='\0';
		return strlen(line);
	}
}

// Function to stripout and replace the $$ in the commandline.
void replaceProcessID(char *line, int ProcNum)
{
	char* Num;
	char* brk;
	char* s;

	brk='\0';
	Num='\0';
	Num = integer_to_string(ProcNum);

	s=malloc(MAXLINE_LENGTH + 1);

	while(strstr(line, "$$") != NULL)
	{
		strncpy(s, line, strcspn(line, "$$"));
		strcat(s, Num);
		brk = strstr(line, "$$");
		// Crop off the "$$"
		++brk; ++brk;
		strcat(s, brk);

		strcpy(line, s);
	}
}

// Function implementations for the built-in functions.
void ProcessEXIT(int ShellProcs[])
{
	// TODO: Shut down all of the background child processes.
	for (int i = 0; i < sizeof(ShellProcs)/sizeof(int); i++)
	{
		if (ShellProcs[i] > 0)
		{
			printf("%i ", ShellProcs[i]);
		}
	}
	
	IsExit = true;
	// printf("\nValue of IsExit is: %i\n", IsExit);
}

int ProcessCD(char* arguments[])
{
	assert(arguments!=NULL);
	char* s;
	char* brk;
	char* TempString;

	s='\0';
	brk='\0';
	TempString = '\0';

	if (arguments[1]== 0)
	{
		// User just typed 'cd', jump to $HOME environment variable.
		s = getenv("HOME");
	}else if (strcmp(arguments[1], "..") == 0)
	{
		// User requested up one level on the directory treefrom PWD.
		s = getenv("PWD");
		TempString = strrchr(s,'/');

		if (strlen(s)>=1 && TempString != NULL)
		{
			*TempString = '\0';
		}
	}else if (strchr(arguments[1], '~') != NULL)
	{
		// user gave referencial path from the Home directory.
		s = malloc(strlen(getenv("HOME"))+1);
		strcpy(s, getenv("HOME"));
		brk = strpbrk(arguments[1],"/");
		strcat(s,brk);
	}

	// DEBUG
	printf("%s\n", s);

	// TODO: CHDIR to the new path location created within the string s.

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
void ParseCommandline(char *arguments[], char* string)
{
	assert(string!=NULL);
	int n_spaces=0;
	char *p = strtok(string, " ");

	// Process the tokens in the string.
	do
	{
		if (strcmp(p, "<") != 0 &&
			strcmp(p, ">") != 0 &&
			strcmp(p, "&") != 0)
		{
			arguments[n_spaces] = p;
		}
		else
		{
			break;
		}

		p = strtok(NULL, " ");
		n_spaces++;
	}while(p != NULL);

	arguments[n_spaces] = NULL;

	return;
}

// quick function to determine of the command being passed in is one that is internal and implemented within this program.
bool IsCommandLineInteral(char* string)
{
	assert(string != NULL);

	return (strcmp(string, "exit")==0 ||
			strcmp(string, "cd")==0 ||
			strcmp(string, "status")==0);
}

// function to spawn a seperate thread via a fork.
void SpawnFork(char* arguments[])
{
	assert(arguments!=NULL);
	pid_t ChildPid;

	switch(ChildPid = fork()){
		case -1:
		CurrentStatus = '\0';
		sprintf(CurrentStatus, "%s %i", "Fork Errored", -1);
			exit(-1);

		case 0:
			// Success start Exec
			SpawnExec(arguments);
			break;

		default:
			// Possibly add the WaitPID() function here.
			break;
	}

}

// Function that will spawn off a Fork and pass through down to exec.
void SpawnForkAndExec(char* arguments[])
{
	assert(arguments!=NULL);

	SpawnExec(arguments);
}

// function to spawn the Exec function.
void SpawnExec(char* arguments[])
{
	assert(arguments!=NULL);
	execvp(arguments[0],arguments);


}

// function to hold for a thread via waitpid.
void SpawnWaitPid(pid_t ProcessID)
{

}


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

// REF: https://stackoverflow.com/questions/36274902/convert-int-to-string-in-standard-c
char* integer_to_string(int x)
{
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer)
    {
         sprintf(buffer, "%d", x);
    }
    return buffer; // caller is expected to invoke free() on this buffer to release memory
}