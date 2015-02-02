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
void GenerateRooms(char* dirRooms);
void LoadRoomNames(char* arrRooms[]);

// Struct definition
struct Room {
  char roomName[16];
  char connection1[16]="";
  char connection2[16]="";
  char connection3[16]="";
  char connection4[16]="";
  char connection5[16]="";
  char connection6[16]="";
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

  char* roomNames[7][16];

  // Generate the game content.
  LoadRoomNames(roomNames);
  GenerateRooms(dirRoom, roomNames);

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

void GenerateRooms(char* dirRooms, char* arrRooms[])
{
  // printf("DEBUG: dirRoom is %s\n", dirRooms);
  for (int i = 0; i < 7; ++i)
  {
    /* code */
  }

  return ;
}

void LoadRoomNames(char* arrRooms[])
{
  for (int i = 0; i < 7; ++i)
  {
    switch i
      case 0:
        strcpy(arrRooms[i][],"Diablo");
        break;
      case 1:
        strcpy(arrRooms[i][],"Crypt");
        break;
      case 2:
        strcpy(arrRooms[i][],"Rivendell");
        break;
      case 3:
        strcpy(arrRooms[i][],"Skyrim");
        break;
      case 4:
        strcpy(arrRooms[i][],"Morrowind");
        break;
      case 5:
        strcpy(arrRooms[i][],"Destiny-SGU");
        break;
      case 6:
        strcpy(arrRooms[i][],"Transporter");
        break;
      case default:
        break;
  }

}