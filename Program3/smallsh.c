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
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int bool;
#define true  1
#define false 0

char[] ParseCommandLine(char[] string);
void SpawnFork(pid_t ProcessID);
void SpawnExec(pid_t ProcessID);
void SpawnWaitPid(pid_t ProcessID);


void main()
{



}

// function to parse out or crack the commandline
char[] ParseCommandLine(char[] string)
{
	char[] ReturnVal;

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

