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

// Global variables and definitions
// Placing the key directory up here for efficiency of use
char RoomDir[64];	// buffer that holds the name of the current directory to use for the game.
char *TempBuf;		// A Temporary string to be passed between the three helper functions for room lookup information.

// Dynamic Array come from (Craig) Allan Reitan's work in CS 261 Data Structures.

#ifndef DYNAMIC_ARRAY_INCLUDED
#define DYNAMIC_ARRAY_INCLUDED 1

# ifndef TYPE
# define TYPE     char *
# define TYPE_SIZE sizeof(char)
# endif

# ifndef EQ
# define EQ(A, B) (fabs(A - B) < 10e-7)
# endif

typedef struct DynArr DynArr;
struct DynArr
{
	TYPE *data;		/* pointer to the data array */
	int size;		/* Number of elements in the array */
	int capacity;	/* capacity ofthe array */
};


DynArr *createDynArr(int cap);
void deleteDynArr(DynArr *v);

int sizeDynArr(DynArr *v);

void addDynArr(DynArr *v, TYPE val);
TYPE getDynArr(DynArr *v, int pos);
void putDynArr(DynArr *v, int pos, TYPE val);
void swapDynArr(DynArr *v, int i, int  j);
void removeAtDynArr(DynArr *v, int idx);

/* Stack interface. */
int isEmptyDynArr(DynArr *v);
void pushDynArr(DynArr *v, TYPE val);
TYPE topDynArr(DynArr *v);
void popDynArr(DynArr *v);

/* Bag Interface */
int containsDynArr(DynArr *v, TYPE val);
void removeDynArr(DynArr *v, TYPE val);

#endif

typedef int bool;
#define true  1
#define false 0

#define MAX_READ 256

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
int MakeTime();
void RenderGameOver(DynArr *v, int Steps);

// Tool or Assistance Functions
void stripLeadingAndTrailingSpaces(char* string);

int main(int argc, char* argv[])
{
	// Initialize Variables to be used in the program
	/******
	Path:	 	Dynamic array that will hold the room names the player traveled through
	Steps_Taken:Counter for the number of steps taken.
	RoomDir:	Char array that will hold the name of the directory to the room files.
	IsGameOver:	True / False 'nuff said.

	*******/
	DynArr *Path;
	Path = createDynArr(2);
	int Steps_Taken = 0; 		// Should always be one step less than the number of elements in path.
	char CurrentRoomFile[128];
	int CurrentRoom;
	bool GameOver = false;
	bool IsValidMove = false;
	char Buf[MAX_READ];			// Dynamic array for holding characters as needed for processing.
	char TestString[MAX_READ];  // Dynamic array for holding characters as needed for processing.
	char RoomSuffix[] = "_room";

	// Going to use for holding the two threads we use for this program.
	// Element 0 will be the main program thread that is running.
	// Element 1 will be the timer when it is called.
	/*
	int pthreadResult;
	pthread_t Threads[2];
	pthread_t MainThreadID, TimerThreadID;	

	MainThreadID = pthread_self();
	Threads[0] = MainThreadID;

	pthreadResult = pthread_create(&TimerThreadID, NULL, MakeTime, NULL);
	if (pthreadResult == 0) Threads[1]=TimerThreadID;

	pthread_mutex_t GameMutex = PTHREAD_MUTEX_INITIALIZER;
	*/
	// End setup of the Thread function and operations.

	// DEBUG Operational Testing

	// Initialization of variables that require a non-null starting point.
	memset(RoomDir, '\0', sizeof(RoomDir));
	memset(CurrentRoomFile, '\0', sizeof(CurrentRoomFile));

	// Setup for the start of the game.
	FindRoomsDir(RoomDir);
	sprintf(CurrentRoomFile, FindStartRoom(RoomDir));

	// Put Start Room on the PATH Stack.
	pushDynArr(Path, CurrentRoomFile);

	// Starting the game up
	do
	{
		RenderRoom(CurrentRoomFile);
		// TODO: SCANF Processing of typed input.
		memset(Buf,'\0', sizeof(Buf));
		scanf("%s", Buf);
		memset(TestString,'\0', sizeof(TestString));
		sprintf(TestString, Buf);

		if (strcmp(Buf,"time")==0)
		{
			// TODO: Process time input and mutex.
			MakeTime();
		}else if (IsValidConnection(CurrentRoomFile, TestString))
		{
			// TODO: If Valid Move push onto Path.
			// TODO: Check if room moving to is going to be end room.
			memset(CurrentRoomFile, '\0', sizeof(CurrentRoomFile));
			sprintf(CurrentRoomFile,"%s", TestString);

			// Add room to the PATH and update count.
			pushDynArr(Path, CurrentRoomFile);
			Steps_Taken++;

			// Check for GAME OVER.
			GameOver = IsGameOver(CurrentRoomFile);
		}else
		{
			printf("HAL 9000 SAYS, \'I'M SORRY I AM AFRAID I JUST CAN\'T DO THAT.\'  TRY AGAIN.\n");
		}
	} while (GameOver = false);

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
	return (GetRoomType(RoomFile)=="END_ROOM") ? true : false;
}

//
void RenderGameOver(DynArr *v, int Steps)
{
	assert(v);
	assert(Steps);
	int i;

	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEP%s.  YOUR PATH TO VICTORY WAS:\n", Steps, (Steps > 1) ? "S" : "");

	for (i = 0; i < Steps+1; ++i)
	{
		printf("%s\n", getDynArr(v, i));
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
  int year, month ,day;
  const char * weekday[] = { "Sunday", "Monday",
                             "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};
  const char * month_name[] = {"January", "February", "March", "April",
							"May", "June", "July", "August",
							"September", "October", "November", "December"};
  char LongDateString[100] = "";
  int FileID;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

/*
  sprintf(LongDateString, " %.2d:%.2d%s , %s, %s %3d, %d\n",
  	((timeinfo->tm_hour-11)>0)?timeinfo->tm_hour-12 : timeinfo->tm_hour, timeinfo->tm_min, ((timeinfo->tm_hour-11)>0)?"pm":"am",
  	weekday[timeinfo->tm_wday], month_name[timeinfo->tm_mon], timeinfo->tm_mday, 
  	1900+timeinfo->tm_year);
  */
  strftime(LongDateString, 100, " %I:%M%p , %A, %B %d, %Y", timeinfo);

  printf("\n%s\n", LongDateString);

  FileID = open("currentTime.txt", O_WRONLY | O_CREAT, 0766);

  if (FileID != -1)
  {
  	write(FileID, asctime(timeinfo), strlen(asctime(timeinfo)));
  	close(FileID);
  }else{
  	printf("ERROR: Unable to open the file currentTime.txt for writing.");
  	return 1;
  }

  return 0;
}

/* ************************************************************************
	Dynamic Array Functions - From CS 261
************************************************************************ */

/* Initialize (including allocation of data array) dynamic array.
	param: 	v		pointer to the dynamic array
	param:	cap 	capacity of the dynamic array
	pre:	v is not null
	post:	internal data array can hold cap elements
	post:	v->data is not null
*/
void initDynArr(DynArr *v, int capacity)
{
	assert(capacity > 0);
	assert(v!= 0);
	v->data = (TYPE *) malloc(sizeof(TYPE) * capacity);
	assert(v->data != 0);
	v->size = 0;
	v->capacity = capacity;	
}

/* Allocate and initialize dynamic array.
	param:	cap 	desired capacity for the dyn array
	pre:	none
	post:	none
	ret:	a non-null pointer to a dynArr of cap capacity
			and 0 elements in it.		
*/
DynArr* createDynArr(int cap)
{
	assert(cap > 0);
	DynArr *r = (DynArr *)malloc(sizeof( DynArr));
	assert(r != 0);
	initDynArr(r,cap);
	return r;
}

/* Deallocate data array in dynamic array. 
	param: 	v		pointer to the dynamic array
	pre:	none
	post:	d.data points to null
	post:	size and capacity are 0
	post:	the memory used by v->data is freed
*/
void freeDynArr(DynArr *v)
{
	if(v->data != 0)
	{
		free(v->data); 	/* free the space on the heap */
		v->data = 0;   	/* make it point to null */
	}
	v->size = 0;
	v->capacity = 0;
}

/* Deallocate data array and the dynamic array ure. 
	param: 	v		pointer to the dynamic array
	pre:	none
	post:	the memory used by v->data is freed
	post:	the memory used by d is freed
*/
void deleteDynArr(DynArr *v)
{
	freeDynArr(v);
	free(v);
}

/* Resizes the underlying array to be the size cap 
	param: 	v		pointer to the dynamic array
	param:	cap		the new desired capacity
	pre:	v is not null
	post:	v has capacity newCap
*/
void _dynArrSetCapacity(DynArr *v, int newCap)
{	
	// For non -C99 compiles
	int i;
	/* FIXME: You will write this function */
	assert(v->size - 1 < newCap);
	TYPE *NewType = malloc(sizeof(TYPE) * newCap);
	assert(NewType);

	// capture the current size of the preserved content
	int Size = v->size;

	for (i = 0; i < Size; i++)
	{
		// Preserve current values in array
		NewType[i] = v->data[i];
	}
	// release Memory of old array
	freeDynArr(v);
	// Add back in the newly created pointer to data.
	v->data = NewType;
	v->size = Size;
	v->capacity = newCap;
}

/* Get the size of the dynamic array
	param: 	v		pointer to the dynamic array
	pre:	v is not null
	post:	none
	ret:	the size of the dynamic array
*/
int sizeDynArr(DynArr *v)
{
	return v->size;
}

/* 	Adds an element to the end of the dynamic array
	param: 	v		pointer to the dynamic array
	param:	val		the value to add to the end of the dynamic array
	pre:	the dynArry is not null
	post:	size increases by 1
	post:	if reached capacity, capacity is doubled
	post:	val is in the last utilized pos in the array
*/
void addDynArr(DynArr *v, TYPE val)
{
	/* FIXME: You will write this function */
	/* Check to see if a resize is necessary */
	if (v->size + 1 >= v->capacity)
		_dynArrSetCapacity(v, 2 * v->capacity);

	v->data[v->size] = val;
	v->size++;
}

/*	Get an element from the dynamic array from a specified pos
	
	param: 	v		pointer to the dynamic array
	param:	pos		integer index to get the element from
	pre:	v is not null
	pre:	v is not empty
	pre:	pos < size of the dyn array and >= 0
	post:	no changes to the dyn Array
	ret:	value stored at index pos
*/

TYPE getDynArr(DynArr *v, int pos)
{
	assert(v->data[pos]);
	assert(v->data[pos] != 0);
	assert(pos < v->size && pos >= 0);
	return v->data[pos];
}

/*	Put an item into the dynamic array at the specified location,
	overwriting the element that was there
	param: 	v		pointer to the dynamic array
	param:	pos		the index to put the value into
	param:	val		the value to insert 
	pre:	v is not null
	pre:	v is not empty
	pre:	pos >= 0 and pos < size of the array
	post:	index pos contains new value, val
*/
void putDynArr(DynArr *v, int pos, TYPE val)
{
	assert(v->data[pos]);
	if (v->data[pos] != 0)
	{
		v->data[pos] = val;
	}
}

/*	Swap two specified elements in the dynamic array
	param: 	v		pointer to the dynamic array
	param:	i,j		the elements to be swapped
	pre:	v is not null
	pre:	v is not empty
	pre:	i, j >= 0 and i,j < size of the dynamic array
	post:	index i now holds the value at j and index j now holds the value at i
*/
void swapDynArr(DynArr *v, int i, int  j)
{
	/* FIXME: You will write this function */
	assert(v->data[i]);
	assert(v->data[j]);
	if (((v->data[i] != 0) || (v->data[j] != 0)) 
		&& (i<v->size && j<v->size))
	{
		TYPE TempVal;
		TempVal = v->data[i];
		v->data[i] = v->data[j];
		v->data[j] = TempVal;
	}
}

/*	Remove the element at the specified location from the array,
	shifts other elements back one to fill the gap
	param: 	v		pointer to the dynamic array
	param:	idx		location of element to remove
	pre:	v is not null
	pre:	v is not empty
	pre:	idx < size and idx >= 0
	post:	the element at idx is removed
	post:	the elements past idx are moved back one
*/
void removeAtDynArr(DynArr *v, int idx)
{
	// for compilers that are not using -C99
	int i;

	assert(v->data[idx]);
	if (idx < v->size)
	{
		for (i = idx; i < v->size; i++)
		{
			v->data[i] = v->data[i + 1];
		}
		v->data[v->size] = 0;
		v->size--;
	}
}

/* ************************************************************************
	Stack Interface Functions
************************************************************************ */

/*	Returns boolean (encoded in an int) demonstrating whether or not the 
	dynamic array stack has an item on it.
	param:	v		pointer to the dynamic array
	pre:	the dynArr is not null
	post:	none
	ret:	1 if empty, otherwise 0
*/
int isEmptyDynArr(DynArr *v)
{
	return (v->size == 0) ? 1 : 0;
}

/* 	Push an element onto the top of the stack
	param:	v		pointer to the dynamic array
	param:	val		the value to push onto the stack
	pre:	v is not null
	post:	size increases by 1
			if reached capacity, capacity is doubled
			val is on the top of the stack
*/
void pushDynArr(DynArr *v, TYPE val)
{
	assert(v != 0);
	addDynArr(v, val);
	//if (v->size == v->capacity)
	//	_dynArrSetCapacity(v, 2 * v->capacity);
	//for (int i = 0; i < v->size; i++)
	//{
	//	v->data[i + 1] = v->data[i];
	//}
	//v->data[v->size] = val;
	//v->size++;
}

/*	Returns the element at the top of the stack 
	param:	v pointer to the dynamic array
	pre:	v is not null
	pre:	v is not empty
	post:	no changes to the stack
*/
TYPE topDynArr(DynArr *v)
{
	return v->data[v->size-1];
}

/* Removes the element on top of the stack 
	param:	v pointer to the dynamic array
	pre:	v is not null
	pre:	v is not empty
	post:	size is decremented by 1
			the top has been removed
*/
void popDynArr(DynArr *v)
{
	removeAtDynArr(v, v->size-1);
}

/* ************************************************************************
	Bag Interface Functions
************************************************************************ */

/*	Returns boolean (encoded as an int) demonstrating whether or not
	the specified value is in the collection
	true = 1
	false = 0
	param:	v		pointer to the dynamic array
	param:	val		the value to look for in the bag
	pre:	v is not null
	pre:	v is not empty
	post:	no changes to the bag
*/
int containsDynArr(DynArr *v, TYPE val)
{
	// for compilers that are not using -C99
	int i;

	for (i = 0; i < v->size; i++)
	{
		if (EQ(v->data[i],val))
		{
			return 1;
		}
	}
	return 0;
}

/*	Removes the first occurrence of the specified value from the collection
	if it occurs
	param:	v		pointer to the dynamic array
	param:	val		the value to remove from the array
	pre:	v is not null
	pre:	v is not empty
	post:	val has been removed
	post:	size of the bag is reduced by 1
*/
void removeDynArr(DynArr *v, TYPE val)
{
	// for compilers that are not using -C99
	int i;

	/*assert(containsDynArr(v, val) == 1);*/
	assert(v->size > 0);

	if (containsDynArr(v, val) == 1)
	{
		for (i = 0; i < v->size; i++)
		{
			if (EQ(v->data[i], val))
			{
				removeAtDynArr(v, i);
			}
		}
	}
}