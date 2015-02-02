/* 
* Allan Reitan CS 344 - Homework 2 Adventure Game
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function definitions
void Introduction();
int RoomConnections();

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

  // Begin the Game for the user.
  Introduction();



  return 0;
}

// Function Build outs
void Introduction()
{
  printf("Welcome to the land of CS 344 Adventure.\n\n");

  return;
}

int RoomConnections()
{
  return (rand() % 4) + 3;
};