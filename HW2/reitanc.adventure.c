/* 
* Allan Reitan CS 344 - Homework 2 Adventure Game
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Function definitions
void Introduction();
int RoomConnections();
void GenerateRooms();

// Struct definition
struct Room {
  char roomName[16];
  char connection[6][16];
  char roomType[12];
};

main()
{
  //  Initialize random seed
  srand(1000);

  // Initialize variables
  const char* dirRoomBase = "reitanc.rooms.";
  char dirPid[16];
  int procPid = getpid();
  sprintf(dirPid, "%u", procPid);

  char* dirRoom;
  dirRoom = malloc(strlen(dirRoomBase) + strlen(dirPid) + 1);
  strcpy(dirRoom, dirRoomBase);
  strcat(dirRoom, dirPid);

  // Generate the game content.
  GenerateRooms(dirRoom);

  // Begin the Game for the user.
  Introduction();


  return 0;
}

// Function Build outs
void Introduction()
{
  printf("Welcome to The Land of CS 344 Adventure.\n\n");
  return;
}

int RoomConnections()
{
  return (rand() % 4) + 3;
}

void GenerateRooms(char* dirRooms)
{
  // printf("DEBUG: dirRoom is %s\n", dirRooms);



  return ;
}