/****************************************
*	CS 344 - Program 4 Key Generator
*	11/19/2017 - by (Craig) Allan Reitan
*
*	Purpose: generate a key file as needed in Program 4
*	use: keygen Legth_of_Text_String
*
*****************************************/
#define _GNU_SOURCE

#include "Program4_lib.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char* argv[])
{
	if (argc == 1 || argc > 2){
		fprintf(stderr, "Error, no arguements deteced.\n use: keygen {int length_of_key}\n");
		exit(1);
	} 

	// int filePtr = -5;
	int k;

	int KeyLength = atoi(argv[1]);
	srand(time(NULL));

	for (int i = 0; i < KeyLength; ++i)
	{
		k = RandomNumber(0, 26);
		k = (k==0)? 32 : k + 64;
		printf("%c",k);
	}
	printf("\n");

	return 0;
}
