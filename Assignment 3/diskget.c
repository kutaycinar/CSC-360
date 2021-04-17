/*
 * diskget.c
 *
 * Name: Kutay Cinar
 * Student Number: V00******
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <netinet/in.h> 
#include "Constants.h"

int fatStarts (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, FATSTART_OFFSET, SEEK_SET);
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

int startingBlock(FILE *inputFile, int offset){
	int value;
	offset = offset + DIRECTORY_START_BLOCK_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int nextBlock(FILE *inputFile, int offset){
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int main ( int argc, char *argv[] )
{
	FILE *inputFile;
	char *filename = NULL;
	char *fileToCopy = NULL;
		
	/* Parse the arguments */
	if ( argc != 3 )
	{
		printf ("Usage: ./diskget disk.img file.ext\n");
		exit(0);
	}

	filename = argv[1];
	fileToCopy = argv[2];
	inputFile = fopen(filename, "r");

	int fileFound = 0;

	int rootDirOffset = rootDirectoryStart(inputFile) * DEFAULT_BLOCK_SIZE;
	int rootDirEntries = rootDirectoryBlocks(inputFile) * 8; //  8 directory entries per 512 byte block

	for (int i = 0; i < rootDirEntries; i++)
	{
		int size = fileSize(inputFile, rootDirOffset);
		// If size is not 0, a file or directory entry must exists
		if(size != 0)
		{
			char filename[30]; // the maximum length of any filename is 30 bytes
			fileName(inputFile, rootDirOffset, filename);
			if(strcmp(filename, fileToCopy)==0)
			{
				fileFound = 1;
				// Copy file to new file
				FILE *outFile;
				outFile = fopen(filename, "w");

				int fatStart = fatStarts(inputFile) * DEFAULT_BLOCK_SIZE;
				int startBlock = startingBlock(inputFile, rootDirOffset);

				int blockOffset = startBlock * FAT_ENTRY_SIZE + fatStart;
				int dataOffset = startBlock * DEFAULT_BLOCK_SIZE;

				// printf("File Size = %d\n", size);
				// printf("DEFAULT_BLOCK_SIZE = %d\n", DEFAULT_BLOCK_SIZE);

				while(size > 0){
					//char fileData[DEFAULT_BLOCK_SIZE];
					int sizeMod = size % DEFAULT_BLOCK_SIZE;
					if (sizeMod == 0){
						sizeMod = DEFAULT_BLOCK_SIZE;
					}
					char *fileData = malloc( sizeof(char) * sizeMod);
					// copy over file data
					fseek(inputFile, dataOffset, SEEK_SET);
					fread(fileData, 1, 	sizeMod, inputFile);
					fwrite(fileData, 1, sizeMod, outFile);

					// increment to next block
					startBlock = nextBlock(inputFile, blockOffset);

					// update offsets
					blockOffset = startBlock * FAT_ENTRY_SIZE + fatStart;
					dataOffset = startBlock * DEFAULT_BLOCK_SIZE;

					// decrement size to move to copy over next block
					size = size - DEFAULT_BLOCK_SIZE;

					free(fileData);
				}

				// close file
				fclose(outFile);

				// no need to continue for loop to check other entries for file
				break;

			}

		}
		rootDirOffset = rootDirOffset + DIRECTORY_ENTRY_SIZE;
	}

	if (fileFound==0){
		printf("File not found\n");
	}

	fclose(inputFile);
	return 0;
}