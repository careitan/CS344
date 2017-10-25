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

// Linked List Deque comes from (Craig) Allan Reitan's work in CS 261 Data Structures.
#ifndef __LISTDEQUE_H
#define __LISTDEQUE_H

# ifndef TYPE
# define TYPE      char *
# define TYPE_SIZE sizeof(char)
# endif
# ifndef LT
# define LT(A, B) ((A) < (B))
# endif

# ifndef EQ
# define EQ(A, B) ((A) == (B))
# endif

/* Double Link*/
struct DLink {
	TYPE value;
	struct DLink * next;
	struct DLink * prev;
};

struct linkedList{
	int size;
	struct DLink *firstLink;
	struct DLink *lastLink;
};

struct linkedList *createLinkedList();

/* Deque Interface */
int 	isEmptyList(struct linkedList *lst);
void  addBackList(struct linkedList *lst, TYPE e);
void 	addFrontList(struct linkedList *lst, TYPE e);

TYPE  frontList(struct linkedList *lst);
TYPE 	backList(struct linkedList *lst);

void  removeFrontList(struct linkedList *lst);
void 	removeBackList(struct linkedList *lst);

void  DisplayList(struct linkedList* lst);
void  _printList(struct linkedList* lst);

/*Bag Interface */
void addList(struct linkedList *lst, TYPE v);
int containsList(struct linkedList *lst, TYPE e);
void removeList(struct linkedList *lst, TYPE e);

#endif

// DoubleLinked List from GitHub.com
// https://gist.github.com/mycodeschool/7429492

struct Node  {
	char* data;
	struct Node* next;
	struct Node* prev;
};

struct Node* head; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(char* x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Inserts a Node at head of doubly linked list
void InsertAtHead(char* x) {
	struct Node* newNode = GetNewNode(x);
	if(head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head; 
	head = newNode;
}

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(char* x) {
	struct Node* temp = head;
	struct Node* newNode = GetNewNode(x);
	if(head == NULL) {
		head = newNode;
		return;
	}
	while(temp->next != NULL) temp = temp->next; // Go To last Node
	temp->next = newNode;
	newNode->prev = temp;
}

//Prints all the elements in linked list in forward traversal order
void Print() {
	struct Node* temp = head;
	// printf("Forward: ");
	while(temp != NULL) {
		printf("%s\n",temp->data);
		temp = temp->next;
	}
	//printf("\n");
}

//Prints all elements in linked list in reverse traversal order. 
void ReversePrint() {
	struct Node* temp = head;
	if(temp == NULL) return; // empty list, exit
	// Going to last Node
	while(temp->next != NULL) {
		temp = temp->next;
	}
	// Traversing backward using prev pointer
	// printf("Reverse: ");
	while(temp != NULL) {
		printf("%s\n",temp->data);
		temp = temp->prev;
	}
	// printf("\n");
}

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
void RenderGameOver(struct linkedList *lst);
void RenderGameEnd(int Steps);

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
	/*
	struct linkedList* Path;
	Path = createLinkedList();
	*/
	head = NULL;
	int Steps_Taken;
	char CurrentRoomFile[128];
	int CurrentRoom;
	bool GameOver = false;
	bool IsValidMove = false;
	char Buf[MAX_READ];			// Dynamic array for holding characters as needed for processing.
	char EvalString[MAX_READ];  // Dynamic array for holding characters as needed for processing.
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
	sprintf(CurrentRoomFile, "%s", FindStartRoom(RoomDir));

	// Put Start Room on the PATH Stack.
	InsertAtTail(CurrentRoomFile);
	// addBackList(Path, CurrentRoomFile);

	// Starting the game up
	Steps_Taken=0;
	do
	{

		RenderRoom(CurrentRoomFile);
		// TODO: SCANF Processing of typed input.
		memset(Buf,'\0', sizeof(Buf));
		scanf("%s", Buf);
		memset(EvalString,'\0', sizeof(EvalString));
		sprintf(EvalString, Buf);

		if (strcmp(Buf,"time")==0)
		{
			// TODO: Process time input and mutex.
			MakeTime();
		}else if (IsValidConnection(CurrentRoomFile, EvalString))
		{
			// TODO: If Valid Move push onto Path.
			// TODO: Check if room moving to is going to be end room.
			memset(CurrentRoomFile, '\0', sizeof(CurrentRoomFile));
			sprintf(CurrentRoomFile,"%s", EvalString);

			// Add room to the PATH and update count.
			// addBackList(Path, CurrentRoomFile);
			InsertAtTail(CurrentRoomFile);
			Steps_Taken++;

			// Check for GAME OVER.
			GameOver = IsGameOver(CurrentRoomFile);
		}else
		{
			printf("\nHAL 9000 SAYS, \'I'M SORRY I AM AFRAID I JUST CAN\'T DO THAT.\'  TRY AGAIN.\n");
		}
	} while (GameOver == false);

	// RenderGameOver(Path);

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

// Function to write out the message at the end of the game.
void RenderGameOver(struct linkedList *lst)
{
	assert(lst);
	int i;

	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEP%s.  YOUR PATH TO VICTORY WAS:\n", lst->size-1, ((lst->size-1) > 1) ? "S" : "");
	DisplayList(lst);
	_printList(lst);

	return;
}

// Alternate function to support the simpler Deque
void RenderGameEnd(int Steps)
{
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEP%s.  YOUR PATH TO VICTORY WAS:\n", Steps, (Steps > 1) ? "S" : "");

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
  strftime(LongDateString, 100, " %I:%M%p, %A, %B %d, %Y", timeinfo);

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
	LL Deque Functions - From CS 261
************************************************************************ */

/*
	initList
	param lst the linkedList
	pre: lst is not null
	post: lst size is 0
*/

void _initList (struct linkedList *lst) {
	lst->size = 0;
	lst->firstLink = NULL;
	lst->lastLink = NULL;
}

/*
 createList
 param: none
 pre: none
 post: firstLink and lastLink reference sentinels
 */

struct linkedList *createLinkedList()
{
	struct linkedList *newList = malloc(sizeof(struct linkedList));
	_initList(newList);
	return(newList);
}

/*
	_addLinkBeforeBefore
	param: lst the linkedList
	param: l the  link to add before
	param: v the value to add
	pre: lst is not null
	pre: l is not null
	post: lst is not empty
*/

/* Adds Before the provided link, l */

void _addLinkBefore(struct linkedList *lst, struct DLink *l, TYPE v)
{
	assert(lst != NULL);
	assert(l != NULL);

	struct DLink *NewLink = malloc(sizeof(struct DLink));

	//Set values for new link
	NewLink->value = v;

	if (lst->firstLink == l)
	{
		//DLink is the first value in the link list.
		NewLink->prev = NULL;
		NewLink->next = l;

		//Adjust the former first link prev to NewLink.
		l->prev = NewLink;

		//Update the LinkList to include the new link.
		lst->firstLink = NewLink;
		lst->size++;
	}
	else
	{
		//GO FISH - have to find the link that you will be inserting.
		struct DLink *Current = lst->firstLink;

		// Walk the link list
		while ((Current->next != lst->lastLink) && (Current != l))
		{
			Current = Current->next;
		}

		if (Current == lst->lastLink)
		{
			// Unable to find the given link in the List to be inserted in front of it so adding to back.
			addBackList(lst, v);
		}
		else
		{
			// Insert DLink at Point in List.
			NewLink->prev = Current->prev;
			NewLink->next = Current->next;

			// Adjust the links prev & next in the list.
			Current->prev->next = NewLink;
			Current->next->prev = NewLink;

			// Update the List Size Count
			lst->size++;
		}
	}

}

/*
	addFrontList
	param: lst the linkedList
	param: e the element to be added
	pre: lst is not null
	post: lst is not empty, increased size by 1
*/

void addFrontList(struct linkedList *lst, TYPE e)
{
	assert(lst != NULL);

	struct DLink *NewLink = malloc(sizeof(struct DLink));

	// Populate new Link
	NewLink->value = e;
	NewLink->next = lst->firstLink;
	NewLink->prev = NULL;

	// Add to the existing list and increment size
	lst->firstLink->prev = NewLink;
	lst->firstLink = NewLink;
	lst->size++;
}

/*
	addBackList
	param: lst the linkedList
	param: e the element to be added
	pre: lst is not null
	post: lst is not empty, increased size by 1
*/

// Additional Reference: https://gist.github.com/mycodeschool/7429492
void addBackList(struct linkedList *lst, TYPE e) {
	assert(lst != NULL);
	struct DLink *NewLink = malloc(sizeof(struct DLink));

	// Populate new Link
	NewLink->value = e;
	NewLink->next = NULL;
	NewLink->prev = NULL;

	// Add to the existing list and increment size
	if (lst->firstLink == NULL)
	{
		lst->firstLink = NewLink;
		return;
	}
	else
	{
		// Size greater than zero, list established.
		struct DLink *Current = lst->firstLink;
		while(Current->next != NULL) Current = Current->next;
		Current->next = NewLink;
		NewLink->prev = Current;
		lst->lastLink = NewLink;
		lst->size++;

		return;
	}
}

/*
	frontList
	param: lst the linkedList
	pre: lst is not null
	pre: lst is not empty
	post: none
*/

TYPE frontList (struct linkedList *lst) {
	assert(lst != NULL);
	assert(lst->size > 0);
	/*temporary return value...you may need to change this */
	return lst->firstLink->value;
}

/*
	backList
	param: lst the linkedList
	pre: lst is not null
	pre: lst is not empty
	post: lst is not empty
*/

TYPE backList(struct linkedList *lst)
{
	assert(lst != NULL);
	assert(lst->size > 0);
	/*temporary return value...you may need to change this */
	return lst->lastLink->value;
}

/*
	_removeLink
	param: lst the linkedList
	param: l the linke to be removed
	pre: lst is not null
	pre: l is not null
	post: lst size is reduced by 1
*/

void _removeLink(struct linkedList *lst, struct DLink *l)
{
	assert(lst != NULL);
	assert(l != NULL);

	if (containsList(lst, l->value) == 1)
	{
		struct DLink *dlinkTemp = lst->firstLink;

		while (dlinkTemp != lst->lastLink)
		{
			if (dlinkTemp == l)
			{
				// Set the previous link's next element to the following element after dlinkTemp.
				dlinkTemp->prev->next = dlinkTemp->next;

				// Set the next link's previous element to the dlinkTemp previous.
				dlinkTemp->next->prev = dlinkTemp->prev;

				//Reduce the size of the Link List Count
				lst->size--;

				// Free the memory for the dlinkTemp
				free(dlinkTemp);

				return;
			}
			else
			{
				dlinkTemp = dlinkTemp->next;
			}
		}
	}
	return;
}

/*
	removeFrontList
	param: lst the linkedList
	pre:lst is not null
	pre: lst is not empty
	post: size is reduced by 1
*/

void removeFrontList(struct linkedList *lst) {
	assert(lst != NULL);

	//Redirect the firstLink pointer
	lst->firstLink = lst->firstLink->next;
	lst->firstLink->prev = NULL;

	// Adjust the size of the Link List
	lst->size--;
}

/*
	removeBackList
	param: lst the linkedList
	pre: lst is not null
	pre:lst is not empty
	post: size reduced by 1
*/

void removeBackList(struct linkedList *lst)
{	
	assert(lst != NULL);

	//Redirect the firstLink pointer
	lst->lastLink = lst->lastLink->prev;
	lst->firstLink->next = NULL;

	// Adjust the size of the Link List
	lst->size--;	
}

/*
	isEmptyList
	param: lst the linkedList
	pre: lst is not null
	post: none
*/

int isEmptyList(struct linkedList *lst) {
	assert(lst!=NULL);
	return (lst->size == 0) ? 1 : 0;
}

// Function specifically for the display of the list at the end game.
void DisplayList(struct linkedList* lst) {
	assert(lst != NULL && lst->size >= 1);

	struct DLink *Current = lst->firstLink;

	do{
		printf("%s\n", Current->value);
		Current = Current->next;
	}while (Current->next != NULL);

}

/* Function to print list
 Pre: lst is not null
 */
void _printList(struct linkedList* lst)
{
	assert(lst != NULL);

	struct DLink *Current = lst->firstLink;

	printf("Here are the values inside of the Linked List: [");
	do
	{
		printf("%s, ", Current->value);
		Current = Current->next;
	}while (Current->next != NULL);
	printf(" Size: %i]\n", lst->size);
}

/* 
	Add an item to the bag
	param: 	lst		pointer to the bag
	param: 	v		value to be added
	pre:	lst is not null
	post:	a link storing val is added to the bag
 */
void addList(struct linkedList *lst, TYPE v)
{
	assert(lst != NULL);
	addBackList(lst, v);
}

/*	Returns boolean (encoded as an int) demonstrating whether or not
	the specified value is in the collection
	true = 1
	false = 0
	param:	lst		pointer to the bag
	param:	e		the value to look for in the bag
	pre:	lst is not null
	pre:	lst is not empty
	post:	no changes to the bag
*/
int containsList (struct linkedList *lst, TYPE e) {
	assert(lst != NULL);

	if (lst->size != 0)
	{
		struct DLink Current1 = *lst->firstLink;
		struct DLink Current2 = *lst->lastLink;

		while ((Current1.next != lst->lastLink) && (Current2.prev != lst->firstLink))
		{
			if ((Current1.value == e) || (Current2.value == e))
			{
				return 1;
			}
			else if (Current1.next == Current2.next)
			{
				// Both are the same link so check one and then return.
				return (Current1.value == e) ? 1 : 0;
			}
			else
			{
				Current1 = *Current1.next;
				Current2 = *Current2.prev;
			}
		}
	}

	/*Replaced default if while loop completes and finds nothing return 0 */
	return 0;
}

/*	Removes the first occurrence of the specified value from the collection
	if it occurs
	param:	lst		pointer to the bag
	param:	e		the value to be removed from the bag
	pre:	lst is not null
	pre:	lst is not empty
	post:	e has been removed
	post:	size of the bag is reduced by 1
*/
void removeList (struct linkedList *lst, TYPE e) {
	assert(lst != NULL);
	assert(lst->size > 0);
	int listSize = lst->size;

	struct DLink *TempLink = malloc(sizeof(struct DLink));

	TempLink->value = e;

	_removeLink(lst, TempLink);

	assert(containsList(lst, e) == 0);
	assert(listSize == (lst->size - 1));
}