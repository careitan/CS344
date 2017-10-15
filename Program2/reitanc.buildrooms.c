/*****
 * OSU Fall 2017 CS 344 - Program 2
 * (Craig) Allan Reitan
 * Adventure game build rooms
 * 10/14/2017
 *****/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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
void ConnectRoom(struct Room x, struct Room y);
bool IsSameRoom(struct Room x, struct Room y);

// Master list of the Room names to be accessible as a Global
char* RoomNames[] = {"Holodeck", "Narnia", "Rivendell", "Mordor", "Moria", "Serenity", "Skyrim", "Bob", "Olympia", "Seattle"};

int main(int argc, char* argv[])
{
	// Initializing key variables and functions for the program.
	srand((unsigned int) time(0));  // Gentlemen, start your random engines.

	int i, j, k, r; // k will be used for indexing a while loop and then auto-incrementing at end of loop.
	int ProcID;

	// Select 7 room at random for the game space.
	for (i = 0; i < 7; ++i)
	{
		do
		{
			r=DM_Friend(0,9);
		}while(RoomExists(RoomNames[r])==true);

		//printf("Room name drawn is: %s\n", RoomNames[r]);
		UsedRooms[i].Name = RoomNames[r];
		UsedRooms[i].Type = MID_ROOM;
	}

	// Set the Start Room and End Room flags
	UsedRooms[0].Type = START_ROOM;
	UsedRooms[6].Type = END_ROOM;

	// Create all connections in graph
	// DEBUG
	printf("About to start the WHILE loop to populate the Rooms.\n");
	while (IsGraphFull() == false)
	{
	  AddRandomConnection();
	}
	printf("Finished the WHILE loop to populate the Rooms.\n");
	
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

  for (i = 0; i < 7; ++i)
  {
  	if (UsedRooms[i].Connections[2] == NULL || UsedRooms[i].Connections[5] == NULL)
  	{
  		ReturnVal = ReturnVal && false;
  	}
  	else if (UsedRooms[i].Connections[5] == NULL)
  	{
  		ReturnVal = ReturnVal && false;
  	}else
  	{
  		ReturnVal = ReturnVal && true;
  	}
  }

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

  while(true)
  {
    A = GetRandomRoom();

    if (CanAddConnectionFrom(A) == true)
      break;
  }

  do
  {
    B = GetRandomRoom();
  }
  while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true);

  ConnectRoom(A, B);
  ConnectRoom(B, A);

  return;
}

// Returns a random Room, does NOT validate if connection can be added
struct Room GetRandomRoom()
{
	return UsedRooms[DM_Friend(0,6)];
}

// Returns true if a connection can be added from Room x, false otherwise
bool CanAddConnectionFrom(struct Room x) 
{
  return x.Connections[6] == NULL;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room x, struct Room y) 
{
  int i, k;

  for (i = 0; i < 6; ++i)
  {
  	if (UsedRooms[i].Name == x.Name)
  	{
  		for (k = 0; k < 5; ++k)
  		{
  			if (UsedRooms[i].Connections[k] == NULL)
  			{
  				UsedRooms[i].Connections[k] = y.Name;
  				continue;
  			}
  		}
  	}else if (UsedRooms[i].Name == y.Name)
  	{
  		for (k = 0; k < 5; ++k)
  		{
  			if (UsedRooms[i].Connections[k] == NULL)
  			{
  				UsedRooms[i].Connections[k] = x.Name;
  				continue;
  			}
  		}
  	}
  }

  return;
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room x, struct Room y) 
{
  return (x.Name == y.Name);
}