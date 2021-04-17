/*
 * diskinfo.c
 *
 * Name: Kutay Cinar
 * Student Number: V00******
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <netinet/in.h> 
#include "Constants.h"


int blockSize (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, BLOCKSIZE_OFFSET, SEEK_SET);
	fread(&value, 1, 2, inputFile);
	// Return value using short
	return htons(value);
}

int blockCount (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, BLOCKCOUNT_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int fatStarts (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, FATSTART_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int fatBlocks (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, FATBLOCKS_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

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

int main ( int argc, char *argv[] )
{
	FILE *inputFile;
	char *filename = NULL;
		
	/* Parse the arguments */
	if ( argc != 2 )
	{
		printf ("Usage: ./diskinfo disk.img\n");
		exit(0);
	}

	filename = argv[1];
	inputFile = fopen(filename, "r");


	/* Check to ensure file matches FS_IDENTIFIER */

	char fs[8];
	fread(fs, 1, 8, inputFile); // CSC360FS

	if(strncmp(fs, FS_IDENTIFIER, 8)!=0){
		printf("Error: Not matching file identifier.\n");
		exit(0);
	}

	/* Print super block information */
	printf("Super block information: \n");
	printf("Block size: %d\n", blockSize(inputFile) );
	printf("Block count: %d\n", blockCount(inputFile) );
	printf("FAT starts: %d\n", fatStarts(inputFile) );
	printf("FAT blocks: %d\n", fatBlocks(inputFile) );
	printf("Root directory start: %d\n", rootDirectoryStart(inputFile) );
	printf("Root directory blocks: %d\n", rootDirectoryBlocks(inputFile) );

	printf("\n");

	/* Print FAT information */

	int freeBlocks = 0;
	int reservedBlocks = 0;
	int allocatedBlocks = 0;

	int startsFat = fatStarts(inputFile) * DEFAULT_BLOCK_SIZE;
	int blocksFat = fatBlocks(inputFile) * FAT_ENTRY_PER_BLOCK;

	// Set file pointer position to after super block information
	fseek(inputFile, startsFat, SEEK_SET);

	for(int i = 0; i < blocksFat; i++)
	{
		// Read from FAT to value 
		int value;
		fread(&value, 1, FAT_ENTRY_SIZE, inputFile);

		// Increment block count variable based on value
		if(htonl(value) == FAT_FREE)
		{
			freeBlocks++;
		}
		else if(htonl(value) == FAT_RESERVED)
		{
			reservedBlocks++;
		}
		else {
			allocatedBlocks++;
		}
	}

	printf("FAT information: \n");
	printf("Free Blocks: %d\n", freeBlocks );
	printf("Reserved Blocks: %d\n", reservedBlocks);
	printf("Allocated Blocks: %d\n", allocatedBlocks);

	fclose(inputFile);

	return 0;
}