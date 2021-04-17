/*
 * diskput.c
 *
 * Name: Kutay Cinar
 * Student Number: V00******
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <netinet/in.h> 
#include "Constants.h"
#include <time.h>

struct tm tm;

int fatStarts (FILE *inputFile) {
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, FATSTART_OFFSET, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

int fatBlocks(FILE *inputFile){
	int value;
	// Set file pointer position and read from file
	fseek(inputFile, FATBLOCKS_OFFSET, SEEK_SET);
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



int startingBlock(FILE *inputFile, int offset){
	int value;
	offset = offset + DIRECTORY_START_BLOCK_OFFSET;
	// Set file pointer position and read from file
	fseek(inputFile, offset, SEEK_SET);
	fread(&value, 1, 4, inputFile);
	// Return value using long
	return htonl(value);
}

void setStatus (FILE *inputFile, int offset){
	offset = offset + DIRECTORY_STATUS_OFFSET;
	fseek(inputFile, offset, SEEK_SET);
	int status = DIRECTORY_ENTRY_FILE + DIRECTORY_ENTRY_USED;
	fwrite(&status, 1, 1, inputFile);
}

void setStartingBlock (FILE *inputFile, int offset, int fatOffset){
	offset = offset + DIRECTORY_START_BLOCK_OFFSET;
	fseek(inputFile, offset, SEEK_SET);
	fatOffset = htonl(fatOffset);
	fwrite(&fatOffset, 1, 4, inputFile);
}

void setNumberOfBlocks (FILE *inputFile, int offset, int newFileBlocks){
	offset = offset + DIRECTORY_BLOCK_COUNT_OFFSET;
	fseek(inputFile, offset, SEEK_SET);
	newFileBlocks = htonl(newFileBlocks);
	fwrite(&newFileBlocks, 1, 4, inputFile);
}

void setFileSize (FILE *inputFile, int offset, int newFileSize){
	offset = offset + DIRECTORY_FILE_SIZE_OFFSET;
	fseek(inputFile, offset, SEEK_SET);
	newFileSize = htonl(newFileSize);
	fwrite(&newFileSize, 1, 4, inputFile);
} 

void setCreateTime (FILE *inputFile, int offset, struct tm tm){
	
	offset = offset + DIRECTORY_CREATE_OFFSET;
	fseek(inputFile, offset, SEEK_SET);

	// Format given from assignment
	int year = htons(tm.tm_year + 1900);
	int month = tm.tm_mon + 1;
	int day = tm.tm_mday;
	int hour = tm.tm_hour;
	int minute = tm.tm_min;
	int second = tm.tm_sec;

	// Read from file modify date format
	fwrite(&year, 1, 2, inputFile);
	fwrite(&month, 1, 1, inputFile);
	fwrite(&day, 1, 1, inputFile);
	fwrite(&hour, 1, 1, inputFile);
	fwrite(&minute, 1, 1, inputFile);
	fwrite(&second, 1, 1, inputFile);
} 


void setModifyTime (FILE *inputFile, int offset, struct tm tm){
	
	offset = offset + DIRECTORY_MODIFY_OFFSET;
	fseek(inputFile, offset, SEEK_SET);

	// Format given from assignment
	int year = htons(tm.tm_year + 1900);
	int month = tm.tm_mon + 1;
	int day = tm.tm_mday;
	int hour = tm.tm_hour;
	int minute = tm.tm_min;
	int second = tm.tm_sec;

	// Read from file modify date format
	fwrite(&year, 1, 2, inputFile);
	fwrite(&month, 1, 1, inputFile);
	fwrite(&day, 1, 1, inputFile);
	fwrite(&hour, 1, 1, inputFile);
	fwrite(&minute, 1, 1, inputFile);
	fwrite(&second, 1, 1, inputFile);
} 


void setFileName(FILE *inputFile, int offset, char *newFile){
	offset = offset + DIRECTORY_FILENAME_OFFSET;
	fseek(inputFile, offset, SEEK_SET);
	fwrite(newFile, 1, 31, inputFile); // file name is 31 bytes
}


int findFreeRootOffset(FILE *inputFile){

	int rootDirOffset = rootDirectoryStart(inputFile) * DEFAULT_BLOCK_SIZE;
	int rootDirEntries = rootDirectoryBlocks(inputFile) * 8; //  8 directory entries per 512 byte block

	for (int i = 0; i < rootDirEntries; i++)
	{
		int status = dirStatus(inputFile, rootDirOffset);
		if(!(status & DIRECTORY_ENTRY_USED))
		{ 
			return rootDirOffset;
		}
		rootDirOffset = rootDirOffset + DIRECTORY_ENTRY_SIZE;
	}

	printf("No free root directory entry available on disk.\n");
	exit(0);
	return -1;

}


int findFreeFatOffset(FILE *inputFile){

	int fatStart = fatStarts(inputFile) * DEFAULT_BLOCK_SIZE;
	int fatEntries = fatBlocks(inputFile) * FAT_ENTRY_PER_BLOCK; 

	for (int i = 0; i < fatEntries; i++)
	{
		int fatBlock = nextBlock(inputFile, fatStart);
		if(fatBlock == FAT_FREE)
		{ 
			return i;
		}
		fatStart = fatStart + FAT_ENTRY_SIZE;
	}

	printf("No free fat entry available on disk.\n");
	exit(0);
	return -1;

}


int main ( int argc, char *argv[] )
{
	FILE *inputFile;
	FILE *outFile;
	char *filename = NULL;
	char *newFile = NULL;
		
	/* Parse the arguments */
	if ( argc != 3 )
	{
		printf ("Usage: ./diskput disk.img file.ext\n");
		exit(0);
	}

	filename = argv[1];
	newFile = argv[2];
	inputFile = fopen(filename, "r+"); // open file read + write

	if ( !(outFile = fopen(newFile, "r")) ){
		printf("File not found\n");
		exit(0);
	}

	// Set time to system current time
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	// New file's size
	fseek(outFile, 0, SEEK_END);
	int newFileSize = ftell(outFile);
	int newFileBlocks = newFileSize / DEFAULT_BLOCK_SIZE + 1;

	// Find spaces to put new file in
	int rootOffset = findFreeRootOffset(inputFile);
	int freeFat = findFreeFatOffset(inputFile);

	// Set directory entry values
	setStatus(inputFile, rootOffset);
	setStartingBlock(inputFile, rootOffset, freeFat);
	setNumberOfBlocks(inputFile, rootOffset, newFileBlocks);
	setFileSize(inputFile, rootOffset, newFileSize);
	setCreateTime(inputFile, rootOffset, tm);
	setModifyTime(inputFile, rootOffset, tm);
	setFileName(inputFile, rootOffset, newFile);


	// Write FAT data
	int fileOffset = 0;
	int fatStart = fatStarts(inputFile) * DEFAULT_BLOCK_SIZE;
	int dataOffset = freeFat * DEFAULT_BLOCK_SIZE;


	// copy over file data
	while(newFileSize > 0){

		int sizeMod = newFileSize % DEFAULT_BLOCK_SIZE;
		if (sizeMod == 0){
			sizeMod = DEFAULT_BLOCK_SIZE;
		}

		char *fileData = malloc( sizeof(char) * sizeMod);
		// read file data
		fseek(outFile, fileOffset, SEEK_SET);
		fread(fileData, 1, sizeMod, outFile);

		// write file data
		fseek(inputFile, dataOffset, SEEK_SET);
		fwrite(fileData, 1, sizeMod, inputFile);

		// increment to next block
		freeFat = findFreeFatOffset(inputFile);

		// update offset
		dataOffset = fatStart + freeFat * DEFAULT_BLOCK_SIZE;

		// decrement file size to move to copy over next block
		newFileSize = newFileSize - DEFAULT_BLOCK_SIZE;
		fileOffset = fileOffset + DEFAULT_BLOCK_SIZE;

		free(fileData);

	}

	fclose(outFile);
	fclose(inputFile);
	return 0;
}