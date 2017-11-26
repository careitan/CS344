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

// Begin Function Definitions
typedef int bool;
#define true  0
#define false 1


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

// https://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c/39475626#39475626
int RandomNumber(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

// Redirect STDIN to the file name provided.
// Returns True / False to indicate if it did so with out any error.
// Function will assume that the full path to the filename is given.
bool RedirectInput(char* FileName)
{
	assert(FileName != NULL);
	int fd = -5;

	fd = open(FileName, O_RDONLY, 0644);
	if (fd==-1){
		fprintf(stderr, "Unable to Open File named: %s\n", FileName);
		return false;
	}

	if (dup2(fd, 0)==-1)
	{
		fprintf(stderr, "Unable to Redirect STDIN to File named: %s\n", FileName);
		return false;
	}else{
		close(fd);
		return true;
	}
}

// Redirect STDOUT to the file name provided.
// Returns True / False to indicate if it did so with out any error.
// Function will assume that the full path to the filename is given.
bool RedirectOutput(char* FileName)
{
	assert(FileName != NULL);
	int fd = -5;

	fd = open(FileName, O_RDONLY, 0644);
	if (fd==-1){
		fprintf(stderr, "Unable to Open File named: %s\n", FileName);
		return false;
	}

	if (dup2(fd, 1)==-1)
	{
		fprintf(stderr, "Unable to Redirect STDOUT to File named: %s\n", FileName);
		return false;
	}else{
		close(fd);
		return true;
	}
}

// Get the file descriptor of the FileName given.  If it is unable to be accessed, then 
int GetFileDescriptorRead(char* FileName)
{   
    int ReturnVal = open(FileName, O_RDONLY);
    if (ReturnVal == -1) fprintf(stderr, "Unable to open file, %s\n",FileName);

    return ReturnVal;
}

// file content length counter based on C Routine found at:
// http://www.opentechguides.com/how-to/article/c/72/c-file-counts.html
bool IsValidFileSet(char* FileName, char* KeyFile)
{
    assert (FileName != NULL && KeyFile != NULL);
    bool ReturnVal = false;
    char ch;

    FILE *FN, *KF;

    int FNCount, KFCount;
    FNCount = 0;
    KFCount = 0;

    FN = fopen(FileName, "r");
    KF = fopen(KeyFile, "r");

    if (FN)
    {
        while((ch=getc(FN)) != EOF){
            if ((ch >= 'A' && ch <= 'Z') || isspace(ch))
            {
                if (ch != '\n') ++FNCount;
            }else{
                fprintf(stderr, "Invalid Character detected in the Source file.\n");
                fclose(FN);
                fclose(KF);
                return 1;
            }
        }
    }else{
        fprintf(stderr, "Failed to open the file for counting: %s\n", FileName);
    }

    if (KF)
    {
        while((ch=getc(KF)) != EOF){
            if ((ch >= 'A' && ch <= 'Z') || isspace(ch))
            {
                if (ch != '\n') ++KFCount;
            }else{
                fprintf(stderr, "Invalid Character detected in the Key file.\n");
                fclose(FN);
                fclose(KF);
                return 1;
            }
        }
    }else{
        fprintf(stderr, "Failed to open the file for counting: %s\n", KeyFile);
    }

    ReturnVal = (FNCount <= KFCount)? 0 : 1;
    fclose(FN);
    fclose(KF);

    // DEBUG
    printf("DEBUG Values, FNCount: %i; KFCount: %i \n", FNCount, KFCount);
    printf("DEBUG Value of ReturnVal %i \n", ReturnVal);

    return ReturnVal;
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

// Error function used for reporting issues
void error(const char *msg) 
{
    perror(msg); 
    exit(0); 
} 
