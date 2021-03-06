/* 
* Allan Reitan CS 344 - Homework 2 Adventure Game
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef BUF_SIZE
#define BUF_SIZE 512
#endif
#define DIR_PERMS (S_IRWXU|S_IRWXG|S_IRWXO)
#define FILE_PERMS (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)

// Function definitions
void Introduction();
int RoomConnections();
void GenerateRooms(char* dirRooms, char* arrRooms[]);
void LoadRoomNames(char* arrRooms[]);
char* RoomFilePath(char* dirRooms, char* strRoom);

// Struct definition
struct Room {
  char roomName[16];
  char connection1[16];
  char connection2[16];
  char connection3[16];
  char connection4[16];
  char connection5[16];
  char connection6[16];
  char roomType[12];
};

int main()
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

  char* roomNames[]={"Diablo", "Crypt", "Rivendell", "Skyrim", "Morrowind", 
  "Destiny-SGU", "Transporter"};

  // Generate the game content.
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

int RoomSelector()
{
  // 0 - 6 in the array.
  return (rand() % 7);
}

void GenerateRooms(char* dirRooms, char* arrRooms[])
{
  // printf("DEBUG: dirRoom is %s\n", dirRooms);
  ssize_t numRead;
  ssize_t numWrite;
  
  mode_t filePerms;

  int i, fd;
  FILE *fp;

  char buf[BUF_SIZE];
  char* room;

  // Make the initial root directory for this program iteration run.
  char* newDir;
  strcpy(newDir,"");
  strcat(newDir,dirRooms);
  //printf("DEBUG newDir set to: %s\n", newDir);

  mkdir(newDir, 0777);
  chmod(newDir, 0777);
  //mkdir(newDir, DIR_PERMS);

  for (i = 0; i < 7; ++i)
  {
    // Initially create all the files that we will be using.
    printf("DEBUG arrRooms[i] set to: %s\n", arrRooms[i]);
    printf("DEBUG Room set to: %s\n", room);
    strcpy(room,"");
    printf("DEBUG Room set to: %s\n", room);
    strcat(room, arrRooms[i]);
    printf("DEBUG Room set to: %s\n", room);

    char* file;
    strcpy(file,"");
    printf("DEBUG file set to: %s\n", file);
    strcat(file,RoomFilePath(dirRooms, room));
    
    //file = RoomFilePath(dirRooms, Room);
    printf("DEBUG char* file set to: %s\n", file);

    // File Pointer Method.
    // if((fp = fopen(file, "w+"))==NULL){
    //   fprintf(stderr, "Cannot open a file: %s\n", file);
    //   exit(1);
    // }else{
    //   strcpy(buf,"");
    //   strcat(buf,"ROOM NAME: ");
    //   strcat(buf,arrRooms[i]);

    //   fputs(buf,fp);
    // }

    // Regular Open Method.
    fd = open(file, O_RDWR|O_CREAT, 0777);
    
    if (fd == -1){
      fprintf(stderr, "Could not open the new file %s\n", file);
    }else{
      strcpy(buf,"");
      strcat(buf,"ROOM NAME: ");
      strcat(buf,arrRooms[i]);
      numWrite = write(fd, buf, BUF_SIZE);
      close(fd);
    }
  }

  for (i = 0; i < 7; ++i)
  {
    // switch(i){
    //   case 0:
    //   // Start_room
    //     break;
    //   case 6:
    //   // End_room
    //     break;
    //   case default:
    //   // MID_ROOM
    //     break;
    //   }
  }

  return ;
}

void LoadRoomNames(char* arrRooms[])
{
  int i;

  for (i = 0; i < 7; ++i)
  {
    // switch(i){
    //   case 0:
    //     break;
    //   case 1:;
    //     break;
    //   case 2:
    //     break;
    //   case 3:;
    //     break;
    //   case 4:
    //     break;
    //   case 5:
    //     break;
    //   case 6:
    //     break;
    //   case default:
    //     break;
    //   }
  }

}

char* RoomFilePath(char* dirRooms, char* strRoom)
{
  char* RetVal;
  //printf("DEBUG Inside the RoomFilePath function.\n");
  strcpy(RetVal,"");
  strcat(RetVal,"~/");
  //strcat(RetVal,"/");
  strcat(RetVal,dirRooms);
  strcat(RetVal,"/");
  strcat(RetVal,strRoom);
  strcat(RetVal,".txt");
  //printf("DEBUG Exiting the RoomFilePath function.\n");
  return RetVal; 
}