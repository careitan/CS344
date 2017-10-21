/*****
 * OSU Fall 2017 CS 344 - Program 2
 * (Craig) Allan Reitan
 * Adventure game build rooms
 * 10/14/2017
 *****/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Creating Typedef because bool is not available in ANSI C
typedef int bool;
#define true  1
#define false 0

#define PERMS 0766	// RW for owner, group, others
#define BUFSIZE 1024 // Setting the default size value for use with buffers.

// Global defined variables or structs
enum Room_TYPE {MID_ROOM = 1, START_ROOM, END_ROOM};

struct Room {
	char* Name;
	char* Connections[6];
	enum Room_TYPE Type;
}UsedRooms[7];

// Function Headers used this file.
int DM_Friend(int min_num, int max_num);  // The Dungeon Master's Friend.

bool IsGraphFull();
bool RoomExists(char* value);
void AddRandomConnection();
struct Room GetRandomRoom();
bool CanAddConnectionFrom(struct Room x);
bool IsAlreadyConnected(struct Room x, struct Room y);
void ConnectRoom(struct Room x, struct Room y);
bool IsSameRoom(struct Room x, struct Room y);
void WriteOutput(int pid);


// Master list of the Room names to be accessible as a Global
char* RoomNames[] = {"Holodeck", "Narnia", "Rivendell", "Mordor", "Moria", "Serenity", "Skyrim", "Bob", "Olympia", "Seattle"};

int main(int argc, char* argv[])
{
	// Initializing key variables and functions for the program.
	srand((unsigned int) time(0));  // Gentlemen, start your random engines.

	int i, j, k, r; // k will be used for indexing a while loop and then auto-incrementing at end of loop.
	
	// https://stackoverflow.com/questions/8166415/how-to-get-the-pid-of-a-process-in-linux-in-c
	// Get the ProcessID and store it in a variable for later use in the program.
	char line[32];
	FILE *cmd = popen("pidof reitanc.buildrooms", "r");

	fgets(line, 32, cmd);
	int ProcID = atoi(strtok(line, " "));

	pclose(cmd);
	// End code to grab the ProcessID for this application.

	// Select 7 room at random for the game space.
	for (i = 0; i < 7; ++i)
	{
		do
		{
			r=DM_Friend(0,9);
		}while(RoomExists(RoomNames[r])==true);

		UsedRooms[i].Name = RoomNames[r];
		for (j = 0; j < 6; ++j)
		{
			UsedRooms[i].Connections[j]="";
		}
		UsedRooms[i].Type = MID_ROOM;
	}

	// Set the Start Room and End Room flags
	UsedRooms[0].Type = START_ROOM;
	UsedRooms[6].Type = END_ROOM;

	// Create all connections in graph
	while (IsGraphFull() == 0)
	{
	  AddRandomConnection();
	}
	
	// Write the Output Files in the corresponding directory
	WriteOutput(ProcID);

	return 0;
};

// https://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c/39475626#39475626
int DM_Friend(int min_num, int max_num)
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

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()  
{
  int i;
  bool ReturnVal = false;

  if (UsedRooms[0].Connections[2] != "" &&
  	UsedRooms[1].Connections[2] != "" &&
  	UsedRooms[2].Connections[2] != "" &&
  	UsedRooms[3].Connections[2] != "" &&
  	UsedRooms[4].Connections[2] != "" &&
  	UsedRooms[5].Connections[2] != "" &&
  	UsedRooms[6].Connections[2] != "") ReturnVal=true;

  return ReturnVal;
}

// Check to see if the name picked is already assigned to a room
bool RoomExists(char* value)
{
	int k;
	bool ReturnVal = false;

	for (k=0; k<7; k++){
		if (UsedRooms[k].Name == value)
		{
			ReturnVal = true;
			return ReturnVal;
		}
		else if (UsedRooms[k].Name == NULL)
		{
			ReturnVal = false;
			return ReturnVal;
		}
	} 

	return ReturnVal;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()  
{
  struct Room A;  //Maybe a struct, maybe global arrays of ints
  struct Room B;

  while(1)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == true)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == false || IsAlreadyConnected(A, B) || IsSameRoom(A, B) == true);

  ConnectRoom(A, B);
  ConnectRoom(B, A);

  return;
}

// Returns a random Room, does NOT validate if connection can be added
struct Room GetRandomRoom()
{
	return UsedRooms[DM_Friend(0,6)];
}

// check to see if room X is already connected to room y
bool IsAlreadyConnected(struct Room x, struct Room y)
{
	int i;
	bool ReturnVal=false;

	for (i = 0; i < 6; ++i)
	{
		if (x.Connections[i]==y.Name) ReturnVal=true;
	}

	return ReturnVal;
}

// Returns true if a connection can be added from Room x, false otherwise
bool CanAddConnectionFrom(struct Room x) 
{
	bool ReturnVal=false;

	if (x.Connections[5] == "") ReturnVal=true;

  return ReturnVal;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room x, struct Room y) 
{
  int i, k;
  bool IsConnected = false;

  // Search to see if there is an existing connection between room x & y.
  for (i = 0; i < 6; ++i)
  {
  	if (x.Connections[i]==y.Name && IsConnected == false) IsConnected = true;
  }

  // Set the new connection from Room x TO Room y.
  if (!IsConnected)
  {
	  for (i = 0; i < 7; ++i)
	  {
	  	if (UsedRooms[i].Name == x.Name)
	  	{
	  		for (k = 0; k < 6; ++k)
	  		{
	  			if (UsedRooms[i].Connections[k] == "")
	  			{
	  				UsedRooms[i].Connections[k] = y.Name;
	  				break;
	  			}
	  		}
	  	}
	  }
	}

  return;
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room x, struct Room y) 
{
	bool ReturnVal = false;

	if (x.Name == y.Name) ReturnVal=true;

  return ReturnVal;
}

// Write the Output files to the corresponding directory requirements
void WriteOutput(int pid)
{
	int i, j;
	int FileID;
	int result;
	int n_read, n_written;
	char directory[250];
	char FileName[250];
	char buf[BUFSIZE];

	// Prep for Loop
	sprintf(directory, "reitanc.rooms.%i", pid);
	result = mkdir(directory, 777);

	for (i = 0; i < 7; ++i)
	{
		//Start out creating the new folder and file object
		sprintf(FileName, "%s/%s_room", directory, UsedRooms[i].Name);
		if ((FileID = open(FileName, O_WRONLY | O_CREAT, 0766)) == -1)
			printf("ERROR: WriteOut was unable to create file: %s", UsedRooms[i].Name);
/*		// Write out the Room Name
		sprintf(buf, "ROOM NAME: %s\n", UsedRooms[i].Name);
		write(FileID, buf, 1024);
		
		// Write the Connection elements
		for (j = 0; i < 6; ++j)
		{
			if (UsedRooms[i].Connections[j] != "") {
				sprintf(buf, "CONNECTION %i: %s\n", j, UsedRooms[i].Connections[j]);
				write(FileID, buf, 1024);
			}
		}
 
		// Write the Room Type
		switch(UsedRooms[i].Type){
			case 1:
				sprintf(buf, "ROOM TYPE: MID_ROOM");
			break;
			case 2:
				sprintf(buf, "ROOM TYPE: START_ROOM");
			break;
			case 3:
				sprintf(buf, "ROOM TYPE: FINISH_ROOM");
			break;
		}
*/
		//write(FileID, buf, 1024);
		close(FileID);
	}

	return;
}