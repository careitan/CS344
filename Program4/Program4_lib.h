/****************************************
*	CS 344 - Program 4 Common Functions Library
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: to hold the commonly used functions and tools for all of the required component programs.
*
*****************************************/
#define _GNU_SOURCE

typedef int bool;
#define true  0
#define false 1


char* integer_to_string(int x);
int RandomNumber(int min_num, int max_num);
bool RedirectInput(char* FileName);
bool RedirectOutput(char* FileName);
int GetFileDescriptorRead(char* FileName);
bool IsValidFileSet(char* FileName, char* KeyFile);
void stripLeadingAndTrailingSpaces(char* string);
void error(const char *msg);
int EncryptData(int FileP, int KeyP);
int DecryptData(int FileP, int KeyP);