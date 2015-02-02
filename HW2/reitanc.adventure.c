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

  // Generate the game content.
  GenerateRooms();

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

void GenerateRooms()
{
  const char* dirRoomBase = "reitanc.rooms.";
  int procid = getpid();
  char* dirPid;
  dirPid = malloc(strlen((char)procid)+1);
  strcpy(dirPid, (char)procid);

  char* dirRoom;
  dirRoom = malloc(strlen(dirRoomBase) + strlen(dirPid) + 1);
  strcpy(dirRoom, dirRoomBase);
  strcat(dirRoom, dirPid);

  printf("DEBUG: dirRoom is %s\n", dirRoom);

  return ;
}