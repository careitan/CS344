/*******************************
* OSU CS344 - Fall Quarter 2017
* Program 3 - Small Shell Project
* (Craig) Allan Reitan
* 10-28-2017
* 
* Assignment is to create a shell engine in C
*
********************************/
#define _GNU_SOURCE
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
void SetCurrentStatus(int StatusVal);

// Utility Functions for executing the program
bool IsValidCommandLine(char* string);
void ParseCommandline(char* arguments[], char* string);
bool IsCommandLineInteral(char* string);
void Redirect(bool ResetIn, bool ResetOut, char* line);
// void SpawnFork(char* arguments[]);
// void SpawnForkAndExec(char* arguments[]);
void SpawnExec(char* arguments[]);
int SpawnExecBG(char* arguments[]);
// void SpawnWaitPid(pid_t ProcessID);
int getlineClean(char *line, int max);
void replaceProcessID(char *line, int ProcNum);
void stripLeadingAndTrailingSpaces(char* string);
char* integer_to_string(int x);

// Supporting Globals to Handle Key resources throughout program.
char* CurrentStatus;			// Holding string for the message of the current status to feed to GetStatus process.
char* StartingPWDENV;			// Holding string for the starting value of PWD.
bool IsExit;
int MainPID;					// Holding variable for the Main Process ID to be passed to child process as needed to support program requirements.

int main(int argc, char* argv[], char* envp[])
{
	IsExit = false;
	int ReturnVal=-1;
	char commandLine[MAXLINE_LENGTH];  // read in the stdin put.
	char* ARGS[516];
	int ShellBgProcs[256];		// Array to hold the Process Threads running.
	bool IsBackgroundProc = false;
	pid_t ChildPid = -5;
	int childExitStatus = -5;

	// TODO: Are we going to need a MUTEX here to handle process and thread locks.
	MainPID = getpid();
	ShellBgProcs[0] = 0;  	// initialize the memory space for the start of the program.
	SetCurrentStatus((int)0);
	StartingPWDENV = getenv("PWD");

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
							// Check for redirected STDIN or STDOUT
							if (strstr(commandLine, "<") != NULL ||
								strstr(commandLine, ">") != NULL)
							{
								Redirect(false, false, commandLine);
							}

							// Check for the background process flags
							IsBackgroundProc = (strcspn(commandLine,"&") < strlen(commandLine)-1) ?  true : false;

							// Success start Exec
							if (IsBackgroundProc)
							{
								SpawnExec(ARGS);
							}else
							{
								SpawnExec(ARGS);
							}
							
							break;

						default:
							// Possibly add the WaitPID() function here.
							fflush(stdout);
							waitpid(ChildPid, &childExitStatus, 0);
							if (WIFEXITED(childExitStatus)) SetCurrentStatus(WEXITSTATUS(childExitStatus));

							break;
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
	char* s;
	char* p=strtok(line, " ");

	Num='\0';
	Num = integer_to_string(ProcNum);

	s=malloc(MAXLINE_LENGTH + 1);

	while(p!=NULL)
	{
		if (strcmp(p,"$$")==0)
		{
			strcat(s, Num);
		}else{
			strcat(s, p);
		}
		strcat(s, " ");
		p=strtok(NULL," ");
	}

	// Clean off the trailing space that may have been added in last iteration of the loop.
	stripLeadingAndTrailingSpaces(s);
	strcpy(line, s);
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
	
	// Set the directory to the Original PWD.
	chdir(StartingPWDENV);

	// Set the IsExit flag in case program returns back up into the while loop.
	IsExit = true;
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

// quick setting function to set the value of Current Status.
void SetCurrentStatus(int StatusVal)
{
	sprintf(CurrentStatus, "exit value %i", StatusVal);
}

// function to parse out the commandLine for validity
bool IsValidCommandLine(char* string)
{
	bool ReturnVal=true;
	int Length = strlen(string);

	if (Length > MAXLINE_LENGTH) ReturnVal = false;	// Exceeded Maximum Line Length.
	if (strcspn(string,"(") < Length || strcspn(string,")") < Length) ReturnVal = false;	// Contains the use of Parens like a Subshell.
	if (strcspn(string,"&") < Length-1) ReturnVal = false; // Use of the '&' somewhere other than in the trailing position of function.										
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

// Used to redirect the Input or Output for Standard IN/OUT
// Bool reset flag is used to indicated the that proces is supposed to clear the 
void Redirect(bool ResetIn, bool ResetOut, char* line)
{
	assert((ResetIn == false && ResetOut == false) &&
			line != NULL);

	int RedirectCase = 0;   // Set to 1 for stdin redirect only; 2 for stdout redirect only. 3 for Both.
	int fd = -5;
	char* InputString;
	char* OutputString;
	char* p;
	char* stack[3];			// House the tokens for the input string.
	int i;
	int ReturnVal;

	InputString = '\0';
	OutputString = '\0';
	p = '\0';

	if (ResetIn) dup2(0, STDIN_FILENO);
	if (ResetOut) dup2(1, STDOUT_FILENO);

	// set input redirect string
	if (strstr(line, "<") != NULL)
	{
		RedirectCase += 1;

		p = strtok(line, "<>");
		i = 0;
		while(p!=NULL)
		{
			stack[i++]=p;
			p=strtok(NULL,"<>");
		}

		// The input string is going to be the part after or between the tokens.
		strcpy(InputString, stack[1]);
		stripLeadingAndTrailingSpaces(InputString);
	}

	// set output redirect string
	if (strstr(line, ">") != NULL)
	{
		RedirectCase += 2;

		p = strtok(line, ">");
		i = 0;
		while(p!=NULL)
		{
			stack[i++]=p;
			p=strtok(NULL,">");
		}

		// The output redirect will be to outside of the edge of the tokens.
		strcpy(OutputString, stack[1]);
		stripLeadingAndTrailingSpaces(OutputString);
	}

	// Based on the value of the Redirect Case, case 1 is stdin; case 2 is stdout; default is both.
	switch(RedirectCase)
	{
		case 1:
			fd = open(InputString, O_RDONLY | O_CLOEXEC);
			if (fd==-1) {
				SetCurrentStatus(fd);
				return;
			}

			ReturnVal = dup2(0, fd);
			close(fd);
			(ReturnVal == -1) ? SetCurrentStatus(ReturnVal) : SetCurrentStatus(0);
			break;
		case 2:
			fd = open(OutputString, O_WRONLY| O_CREAT | O_TRUNC, 0644);
			if (fd==-1) {
				SetCurrentStatus(fd);
				return;
			}

			ReturnVal = dup2(1, fd);
			close(fd);
			(ReturnVal == -1) ? SetCurrentStatus(ReturnVal) : SetCurrentStatus(0);
			break;
		default:
			fd = open(InputString, O_RDONLY | O_CLOEXEC);
			if (fd==-1) {
				SetCurrentStatus(fd);
				return;
			}

			ReturnVal = dup2(0, fd);
			close(fd);
			(ReturnVal == -1) ? SetCurrentStatus(ReturnVal) : SetCurrentStatus(0);

			fd = open(OutputString, O_WRONLY| O_CREAT | O_TRUNC, 0644);
			if (fd==-1) {
				SetCurrentStatus(fd);
				return;
			}

			ReturnVal = dup2(1, fd);
			close(fd);
			(ReturnVal == -1) ? SetCurrentStatus(ReturnVal) : SetCurrentStatus(0);
			break;
	}

	return;
}

// function to spawn a seperate thread via a fork.
/*
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
*/

// Function that will spawn off a Fork and pass through down to exec.
/*
void SpawnForkAndExec(char* arguments[])
{
	assert(arguments!=NULL);

	SpawnExec(arguments);
}
*/

// function to spawn the Exec function.
void SpawnExec(char* arguments[])
{
	int ReturnVal;
	assert(arguments!=NULL);
	ReturnVal = execvp(arguments[0],arguments);

	if (ReturnVal == -1)
	{
		SetCurrentStatus(-1);
	}else{
		SetCurrentStatus(0);
	}
}

// function to spawn the Exec function with a background process and return the Process ID for storage in the Array.
int SpawnExecBG(char* arguments[])
{

	return 0;
}

// function to hold for a thread via waitpid.
/*
void SpawnWaitPid(pid_t ProcessID)
{

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