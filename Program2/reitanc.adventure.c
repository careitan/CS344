/******
 * OSU Fall 2017 CS 344 - Program 2
 * (Craig) Allan Reitan
 * Adventure File for program implementation
 * 10/14/2017
 *****/

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
#include <time.h>
#include <unistd.h>

typedef int bool;
#define true  1
#define false 0

#define MAX_READ 256
#define MAX_READBACK 4096

// Global variables and definitions
// Placing the key directory up here for efficiency of use
char RoomDir[64];	// buffer that holds the name of the current directory to use for the game.
char *TempBuf;		// A Temporary string to be passed between the three helper functions for room lookup information.
char WriteBuf[MAX_READ]; // A Temporary string to be used for staging content for writing out to the Path file.
char PathStr[MAX_READBACK];

// Functions used to implement CS 344 - Program 2 assignment requirements
void FindRoomsDir(char s[]);
char* FindStartRoom(char s[]);
char* GetRoomName(char RoomFile[]);
char* GetRoomType(char RoomFile[]);
char* GetRoomConnection(char RoomFile[], int ConnectionNumber);
int GetRoomFile(char RoomFile[]);
bool IsValidConnection(char RoomFile[], char Move[]);
bool ValidConnection(char RoomFile[], int ConnNumber);

// Game control functions
void RenderRoom(char RoomFile[]);
bool IsGameOver(char RoomFile[]);
int CreatePath(char RoomFile[], char FileName[]);
int WritePath(char RoomFile[], char FileName[]);
int MakeTime();
void RenderGameOver(int Steps, char Path[]);

// Tool or Assistance Functions
void stripLeadingAndTrailingSpaces(char* string);

int main(int argc, char* argv[])
{
	// Initialize Variables to be used in the program
	/******
	Steps_Taken:		Counter for the number of steps taken.
	TempFileID:			Int value for the FileID of the Tempfile that will hold the path string.
	RoomDir:			Char array that will hold the name of the directory to the room files.
	CurrentRoomFile: 	The Short name of the Room file that the game is currently in.
	IsValidMove:		True / False check to see if the new move is valid.
	IsGameOver:			True / False 'nuff said.

	*******/
	int Steps_Taken;
	char TempFile[] = "Pathfile";
	int TempFileID;
	char CurrentRoomFile[128];
	int CurrentRoom;
	bool GameOver = false;
	bool IsValidMove = false;
	char Buf[MAX_READ];				// Dynamic array for holding characters as needed for processing.
	char EvalString[MAX_READ]; 		// Dynamic array for holding characters as needed for processing.
	char RoomSuffix[] = "_room"; 	// Used to quickly append to the CurrentRoomFile to get the file name.

	// Going to use for holding the two threads we use for this program.
	// Element 0 will be the program thread that is going to write to Pathfile.
	// Element 1 will be the timer when it is called.
	/*
	int pthreadResult;
	pthread_t Threads[2];
	pthread_t MainThreadID, TimerThreadID;
	char 

	MainThreadID = pthread_self();
	Threads[0] = MainThreadID;

	pthreadResult = pthread_create(&TimerThreadID, NULL, MakeTime, NULL);
	if (pthreadResult == 0) Threads[1]=TimerThreadID;

	pthread_mutex_t GameMutex = PTHREAD_MUTEX_INITIALIZER;
	// End setup of the Thread function and operations.
*/
	// DEBUG Operational Testing

	// Initialization of variables that require a non-null starting point.
	memset(RoomDir, '\0', sizeof(RoomDir));
	memset(CurrentRoomFile, '\0', sizeof(CurrentRoomFile));

	// Setup for the start of the game.
	FindRoomsDir(RoomDir);
	sprintf(CurrentRoomFile, "%s", FindStartRoom(RoomDir));

	// Put Start Room on the PATH Stack.
	CreatePath(CurrentRoomFile, TempFile);

	// Starting the game up
	Steps_Taken=0;
	do
	{
		// Arrival in room.
		RenderRoom(CurrentRoomFile);

		// Get user input for room move
		memset(Buf,'\0', sizeof(Buf));
		scanf("%s", Buf);

		// Process user input into a string array for passing around in functions if needed.
		memset(EvalString,'\0', sizeof(EvalString));
		sprintf(EvalString, Buf);

		if (strcmp(Buf,"time")==0)
		{
			// TODO: Process time input and mutex.
			MakeTime();
		}else if (IsValidConnection(CurrentRoomFile, EvalString))
		{
			// Room is valid for moving and set the next step.
			memset(CurrentRoomFile, '\0', sizeof(CurrentRoomFile));
			sprintf(CurrentRoomFile,"%s", EvalString);

			// Add room to the PATH and update step count.
			WritePath(CurrentRoomFile, TempFile);
			Steps_Taken++;

			// Check for GAME OVER, and set flag for while loop.
			GameOver = IsGameOver(CurrentRoomFile);
		}else
		{
			// User inputed something that didn't match.
			printf("\nHAL 9000 SAYS, \'I'M SORRY I AM AFRAID I JUST CAN\'T DO THAT.\'  TRY AGAIN.\n");
		}
	} while (GameOver == false);

	RenderGameOver(Steps_Taken, TempFile);

	remove(TempFile);

	//pthread_mutex_destroy(&GameMutex);

	return 0;
};

// Functions used to implement CS 344 - Program 2 assignment requirements

// Tool or Helper Functions
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

// FindRoomsFiles - Find the most current directory based on File Mod property and populate the char array.
void FindRoomsDir(char s[])
{
	// Based on the code sample provided by CS344 Professor Benjamin Brewster
	// https://oregonstate.instructure.com/courses/1648339/pages/2-dot-4-manipulating-directories
	// Modifying it to pass in the char array that will be populated witht he most recent directory.
	int newestDirTime = -1; // Modified timestamp of newest subdir examined
	char targetDirPrefix[32] = "reitanc.rooms."; // Prefix we're looking for

	DIR* dirToCheck; // Holds the directory we're starting in
	struct dirent *fileInDir; // Holds the current subdir of the starting dir
	struct stat dirAttributes; // Holds information we've gained about subdir	

	dirToCheck = opendir("."); // Open up the directory this program was run in

	if (dirToCheck > 0) // Make sure the current directory could be opened
	{
	  while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
	  {
	    if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
	    {
	      // printf("Found the prefex: %s\n", fileInDir->d_name);
	      stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

	      if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
	      {
	        newestDirTime = (int)dirAttributes.st_mtime;
	        memset(s, '\0', sizeof(s));
	        strcpy(s, fileInDir->d_name);
	        // printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);
	      }
	    }
	  }
	}
	closedir(dirToCheck); // Close the directory we opened

	// printf("Newest entry found is: %s\n", s);
}

// Use this function to locate the Start Room and Populate the CurrentRoomFile Name Global to start the game.
char* FindStartRoom(char s[])
{
	DIR* dirToCheck; // Holds the directory we're starting in
	char targetFileSuffix[6] = "_room";
	struct dirent *fileInDir; // Holds the current subdir of the starting dir
	char *TestString;
	TestString="";

	char *ReturnVal;
	ReturnVal="";

	if (strlen(RoomDir) > 1)
	{
		dirToCheck = opendir(RoomDir);
	}else if (strlen(s) > 1){
		dirToCheck = opendir(s);
	}

	if (dirToCheck > 0) // Make sure the current directory could be opened
	{
	  while ((fileInDir = readdir(dirToCheck)) != NULL && strlen(ReturnVal) == 0) // Check each entry in dir
	  {
	  	TestString = fileInDir->d_name;
	    if (strstr(TestString, targetFileSuffix) != NULL) // If entry has suffix
	    {
	      ReturnVal = (strcmp(GetRoomType(TestString),"START_ROOM") == 0) ? GetRoomName(TestString) : "";
	    }
	  }
	}
	closedir(dirToCheck); // Close the directory we opened

	return ReturnVal;
}

// Functions to process the files for a room
// Get the Room Type for a given room name.
char* GetRoomName(char RoomFile[])
{
	char *ReturnVal;
	char TestString[16] = "ROOM NAME";

	// File descriptor of the room that will be rendered.
	int RoomContents = GetRoomFile(RoomFile);
	char Buffer[MAX_READ];
	char *pch;

	if (RoomContents != -1)
	{
		memset(Buffer, '\0', sizeof(Buffer));
		read(RoomContents, Buffer, MAX_READ);
		close(RoomContents);

		// Temp Element List to catch the Tokens.
		// http://www.cplusplus.com/reference/cstring/strtok/
		pch = strtok(Buffer, ":\n");

		while(pch != NULL)
		{
			if (strcmp(pch , TestString) == 0)
			{
				pch = strtok(NULL, ":\n");
				TempBuf="";
				TempBuf=pch;
				stripLeadingAndTrailingSpaces(TempBuf);
				return ReturnVal = TempBuf;
			}

			// cut to the next element in the token list.
			pch = strtok(NULL, ":\n");
		}

	}

	return ReturnVal;
}

char* GetRoomType(char RoomFile[])
{
	char *ReturnVal;

	// File descriptor of the room that will be rendered.
	int RoomContents = GetRoomFile(RoomFile);
	char Buffer[MAX_READ];
	char TestString[16] = "ROOM TYPE";
	char *pch;

	if (RoomContents != -1)
	{
		memset(Buffer, '\0', sizeof(Buffer));
		read(RoomContents, Buffer, MAX_READ);
		close(RoomContents);

		// Temp Element List to catch the Tokens.
		// http://www.cplusplus.com/reference/cstring/strtok/
		pch = strtok(Buffer, ":\n");

		while(pch != NULL)
		{
			if (strcmp(pch, TestString) == 0)
			{
				pch = strtok(NULL, ":\n");
				TempBuf="";
				TempBuf=pch;
				stripLeadingAndTrailingSpaces(TempBuf);
				return ReturnVal = TempBuf;
			}

			// cut to the next element in the token list.
			pch = strtok(NULL, ":\n");
		}

	}

	return ReturnVal;
}

// Use number 1 based for selecting the connection number.
char* GetRoomConnection(char RoomFile[], int ConnectionNumber)
{
	char *ReturnVal;
	char Buffer[MAX_READ];
	char TestString[32];
	char *pch;

	// File descriptor of the room that will be rendered.
	int RoomContents = GetRoomFile(RoomFile);
	ReturnVal="";

	if (strcmp(RoomFile, "") == 0 || RoomFile == NULL) return ReturnVal = "";
	ConnectionNumber = (ConnectionNumber < 1) ? 1 : ConnectionNumber;

	if (RoomContents != -1)
	{
		memset(Buffer, '\0', sizeof(Buffer));
		read(RoomContents, Buffer, MAX_READ);
		close(RoomContents);

		memset(TestString, '\0', sizeof(TestString));
		sprintf(TestString, "CONNECTION %i", ConnectionNumber);
		// Temp Element List to catch the Tokens.
		// http://www.cplusplus.com/reference/cstring/strtok/
		pch = strtok(Buffer, ":\n");

		while(pch != NULL)
		{
			if (strcmp(pch, TestString) == 0)
			{
				pch = strtok(NULL, ":\n");
				TempBuf="";
				TempBuf=pch;
				stripLeadingAndTrailingSpaces(TempBuf);
				// printf("\nDEBUG - GetRoom is Returning:  %s", TempBuf);
				ReturnVal = TempBuf;
				return ReturnVal;
			}

			// cut to the next element in the token list.
			pch = strtok(NULL, ":\n");
		}
	}

	return ReturnVal;
}

int GetRoomFile(char RoomFile[])
{
	int fil;

	char FilePath[sizeof(RoomDir) + strlen(RoomFile) + 1];
	if (strcmp(RoomFile,"") == 0 || RoomFile == NULL) return -1;

	// Check to see if the room name passed in included the suffix '_room'.
	// Actual file names will have '_room'
	sprintf(FilePath, "%s/%s%s", RoomDir, RoomFile, strstr(RoomFile, "_room")==NULL ? "_room" : "");

	// Open the File and return the File Descriptor Int to the calling process.
	fil = open(FilePath, O_RDONLY);
	if (fil == -1)
	{
		printf("ERROR - Unable to Open Room File: %s", RoomFile);
	}

	return fil;
}

bool IsValidConnection(char RoomFile[], char *Move)
{
	bool ReturnVal = false;
	char *TestVal;
	int i;

	// Loop through the Rooms to check and see what value is returned with each connection.
	for (i = 1; i < 7; ++i)
	{
		TestVal = "";
		TestVal = GetRoomConnection(RoomFile, i);
		if (strlen(TestVal) != 0 && strcmp(TestVal, Move) == 0)
		{
			return true;
		} else if (strlen(TestVal) == 0)
		{
			return false;
		}
	}

	return ReturnVal;
}

bool ValidConnection(char RoomFile[], int ConnNumber)
{
	bool ReturnVal=false;
	char *TestVal;

	TestVal = "";
	TestVal = GetRoomConnection(RoomFile, ConnNumber);
	if (strlen(TestVal) != 0 && strcmp(TestVal,"") != 0)
	{
		return true;
	} else if (strlen(TestVal) == 0 || strcmp(TestVal,"") == 0)
	{
		return false;
	}

	return ReturnVal;
}

// Game control functions
// write out the content on the game screen to tell the user where they are and list options to move to.
void RenderRoom(char RoomFile[])
{
	// Iterator that will be used for going through the Connections.
	int i;
	char TestVal[64];
	bool IsConnValid = false;

	// Start the process of reading and rendering the contents of the room.
	printf("\nCURRENT LOCATION: %s\n", GetRoomName(RoomFile));
	
	// Section to write out the Connections Choices.
	printf("POSSIBLE CONNECTIONS: ");
	for (i = 1; i < 7; ++i)
	{
		IsConnValid = ValidConnection(RoomFile, i);
		if (IsConnValid)
		{
			memset(TestVal,'\0', 64);
			sprintf(TestVal, "%s", GetRoomConnection(RoomFile, i));

			switch(i)
			{
				case 1:
					printf("%s",TestVal);
				break;
				default:
					printf(", %s",TestVal);
				break;
			}
		}
	}
	printf(".\n");

	// Final Line on the Room Display.
	printf("WHERE TO? >");
}

// Just check and see if the room moving into is an END_ROOM
bool IsGameOver(char RoomFile[])
{
	return (strcmp(GetRoomType(RoomFile),"END_ROOM") == 0) ? true : false;
}

// Quick function to write out the temp file for the path tracking.
int CreatePath(char RoomFile[], char FileName[])
{
	assert(strlen(RoomFile)>0 && strlen(FileName)>0);

	int fil;
	int ReturnVal;

	if ((fil = open(FileName, O_WRONLY | O_CREAT | O_TRUNC, 0766)) != -1)
	{
		memset(WriteBuf,'\0',strlen(RoomFile)+1);
		sprintf(WriteBuf, "%s\n", RoomFile);
		ReturnVal = write(fil, WriteBuf, strlen(WriteBuf));
		close(fil);
	} else {
		printf("ERROR: Unable to Open or Create temp path file.");
	}

	return ReturnVal;
}

// Quick function to write out the temp file for the path tracking.
int WritePath(char RoomFile[], char FileName[])
{
	assert(strlen(RoomFile)>0 && strlen(FileName)>0);

	int fil;
	int ReturnVal;

	if ((fil = open(FileName, O_WRONLY | O_APPEND, 0766)) != -1)
	{
		memset(WriteBuf,'\0',strlen(RoomFile)+1);
		sprintf(WriteBuf, "%s\n", RoomFile);
		ReturnVal = write(fil, WriteBuf, strlen(WriteBuf));
		close(fil);
	} else {
		printf("ERROR: Unable to Open or Create temp path file.");
	}

	return ReturnVal;
}

// Function to write out the message at the end of the game.
void RenderGameOver(int Steps, char Path[])
{
	assert(strlen(Path)>=0);
	int fil;

	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEP%s.  YOUR PATH TO VICTORY WAS:\n", Steps, (Steps > 1) ? "S" : "");

	if ((fil = open(Path, O_RDONLY)) != -1)
	{
		read(fil, PathStr, MAX_READBACK);
		close(fil);

		printf("%s\n", PathStr);
	} else {
		printf("ERROR: Unable to Open or Access temp path file.");
	}

	return;
}

// The timer function that is used with the MUTEX implementation to demonstrate a second thread.
// When user types a move of 'time', this function will be started up on the second thread and do the following:
// 1. Populate a string with the system time.
// 2. Print it out on the screen.
// 3. Write it out to a local file called 'currentTime.txt'
// REF: http://www.cplusplus.com/reference/ctime/localtime/
int MakeTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  char LongDateString[100] = "";
  int FileID;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(LongDateString, 100, " %I:%M%p, %A, %B %d, %Y", timeinfo);

  printf("\n%s\n", LongDateString);

  FileID = open("currentTime.txt", O_WRONLY | O_CREAT, 0766);

  if (FileID != -1)
  {
  	write(FileID, LongDateString, strlen(LongDateString));
  	close(FileID);
  }else{
  	printf("ERROR: Unable to open the file currentTime.txt for writing.");
  	return 1;
  }

  return 0;
}
