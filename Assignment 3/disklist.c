/*
 * disklist.c
 *
 * Name: Kutay Cinar
 * Student Number: V00******
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <netinet/in.h> 
#include "Constants.h"

int rootDirectoryStart (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, ROOTDIRSTART_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int rootDirectoryBlocks (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, ROOTDIRBLOCKS_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int dirStatus (FILE *inputFile, int offset) {
	int value = 0;
	offset = offset + DIRECTORY_STATUS_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(&value, 1, 1, inputFile);
	// Return value using short
	return value;
}


int fileSize(FILE *inputFile, int offset){
	int value;
	offset = offset + DIRECTORY_FILE_SIZE_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}


void fileName(FILE *inputFile, int offset, char *filename){
	offset = offset + DIRECTORY_FILENAME_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(filename, 1, 31, inputFile);
}

void modifiedDate(FILE *inputFile, int offset){
	offset = offset + DIRECTORY_MODIFY_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	// Format given from assignment
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	// Read from file modify date format
	fread(&year, 1, 2, inputFile);
	fread(&month, 1, 1, inputFile);
	fread(&day, 1, 1, inputFile);
	fread(&hour, 1, 1, inputFile);
	fread(&minute, 1, 1, inputFile);
	fread(&second, 1, 1, inputFile);

	printf("%d/%02d/%02d %02d:%02d:%02d", htons(year),month,day, hour, minute, second);
}

int main ( int argc, char *argv[] )
{
	FILE *inputFile;
	char *filename = NULL;
		
	/* Parse the arguments */
	if ( argc != 2 )
	{
		printf ("Usage: ./disklist disk.img\n");
		exit(0);
	}

	filename = argv[1];
	inputFile = fopen(filename, "r");

	int rootDirOffset = rootDirectoryStart(inputFile) * DEFAULT_BLOCK_SIZE;
	int rootDirEntries = rootDirectoryBlocks(inputFile) * 8; //  8 directory entries per 512 byte block

	for (int i = 0; i < rootDirEntries; i++)
	{
		int size = fileSize(inputFile, rootDirOffset);
		// If size is not 0, a file or directory entry must exists
		if(size != 0)
		{
			int status = dirStatus(inputFile, rootDirOffset);
			char filename[30]; // the maximum length of any filename is 30 bytes
			fileName(inputFile, rootDirOffset, filename);
			if(status & DIRECTORY_ENTRY_FILE)
			{ 
				printf("F ");
			}
			else
			{
				printf("D ");
			}
			printf("%10d ", size);
			printf("%30s ", filename);
			modifiedDate(inputFile, rootDirOffset);
			printf("\n");

		}
		rootDirOffset = rootDirOffset + DIRECTORY_ENTRY_SIZE;
	}

	fclose(inputFile);

	return 0;
}