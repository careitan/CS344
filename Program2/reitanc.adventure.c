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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Global variables and definitions
// Placing the key directory up here for efficiency of use
char RoomDir[64];

// Dynamic Array come from (Craig) Allan Reitan's work in CS 261 Data Structures.

#ifndef DYNAMIC_ARRAY_INCLUDED
#define DYNAMIC_ARRAY_INCLUDED 1

# ifndef TYPE
# define TYPE     double 
# define TYPE_SIZE sizeof(double)
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

/* Dynamic Array Functions */
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

// Functions used to implement CS 344 - Program 2 assignment requirements
void FindRoomsFiles(char s[]);
char* GetRoomName(char RoomFile[]);
char* GetRoomType(char RoomFile[]);
char* GetRoomConnection(char RoomFile[], int ConnectionNumber);
FILE* GetRoomFile(char RoomFile[]);
bool IsValidConnection(char RoomFile[], char Move[]);
bool ConnectionExists(char RoomFile[], int ConnNumber);

// Game control functions
void RenderRoom(char RoomFile[]);
bool IsGameOver(char RoomFile[]);


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
	int Steps_Taken = 0;
	char CurrentRoomFile[256];
	bool IsGameOver = false;
	
	// Initialization of variables that require a non-null starting point.
	memset(RoomDir, '\0', sizeof(RoomDir));

	FindRoomsFiles(RoomDir);


	return 0;
};

// Functions used to implement CS 344 - Program 2 assignment requirements

// FindRoomsFiles - Find the most current directory based on File Mod property and populate the char array.
void FindRoomsFiles(char s[])
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

// Functions to process the files for a room
// Get the Room Type for a given room name.
char* GetRoomName(char RoomFile[])
{
	char *ReturnVal;
	return ReturnVal;
}

char* GetRoomType(char RoomFile[])
{
	char *ReturnVal;
	return ReturnVal;
}

char* GetRoomConnection(char RoomFile[], int ConnectionNumber)
{
	char *ReturnVal;
	return ReturnVal;
}

FILE* GetRoomFile(char RoomFile[])
{
	FILE* fil;
	return fil;
}

bool IsValidConnection(char RoomFile[], char Move[])
{
	bool ReturnVal=false;
	return ReturnVal;
}

bool ConnectionExists(char RoomFile[], int ConnNumber)
{
	bool ReturnVal=false;
	return ReturnVal;
}

// Game control functions
// write out the content on the game screen to tell the user where they are and list options to move to.
void RenderRoom(char RoomFile[])
{

}

bool IsGameOver(char RoomFile[])
{
	return (GetRoomType(RoomFile)=="FINISH_ROOM") ? true : false;
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