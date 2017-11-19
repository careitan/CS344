/****************************************
*	CS 344 - Program 4 Common Functions Library
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: to hold the commonly used functions and tools for all of the required component programs.
*
*****************************************/
#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


typedef int bool;
#define true  0
#define false 1