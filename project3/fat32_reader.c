/***********************************************************
 * Name of program: fat32_reader.c
 * Authors:			Ethan Morisette and Kaleb Luse
 * Description:		a shell interface to a fat32 file system image
 **********************************************************/

/* These are the included libraries.  You may need to add more. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "directory_entries.h"
#include "equations.h"

//////////////////////////////////////////////////////
// MACROS ////////////////////////////////////////////
//////////////////////////////////////////////////////

#define BOOL int
#define TRUE 1
#define FALSE 0
#define MAX_CMD 80

//////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////
//////////////////////////////////////////////////////////
FILE* 		filePtr;				// pointer to the fat32 image file
bootSector  thisBootSector;
uint64_t 	currentByteAddress;		// stores the byte address of the current directory

//////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES ///////////////////////////////////
//////////////////////////////////////////////////////////
void printInfo();
void printVolumeName();
void printStats(char* fileName);
void listFiles();
void changeDirectory(char* directory);
void readFile();

/* This is the main function of your project, and it will be run
 * first before all other functions.
 */
int main(int argc, char *argv[])
{
	char 		cmd_line[MAX_CMD];		// command line container for our file system shell
	uint32_t 	fatSectorNumber;		//TODO add comment
	uint16_t 	fatEntryOffset;			//
	uint32_t 	rootFirstSectorNum;		//

	/* Parse args and open our image file */
	if (argc != 2) {
		printf("You entered an incorrect amount of arguments for this program. <USAGE: ./fat32_reader fat32.img>\n");
		return -1;
	}

	if ((filePtr = fopen(argv[1], "rb")) == 0) {
		perror(argv[1]);
		return -1;
	}

	thisBootSector = setUpBootSector(filePtr);

	/* Get root directory address */
	rootFirstSectorNum = getFirstSectorOfCluster(thisBootSector, thisBootSector.rootClusterNum);
	currentByteAddress = convertSectorNumToBytes(thisBootSector, rootFirstSectorNum);
	printf("Root addr is hex: 0x%x, dec: %d\n", currentByteAddress, currentByteAddress);


	/* Main loop.  You probably want to create a helper function
       for each command besides quit. */

	while(TRUE) {
		bzero(cmd_line, MAX_CMD);
		printf("/]");
		fgets(cmd_line,MAX_CMD,stdin);

		/* Start comparing input */
		if(strncmp(cmd_line,"info",4)==0) {
			printInfo();
		}

		else if(strncmp(cmd_line,"volume",6)==0) {
			printVolumeName();
		}
		
		else if(strncmp(cmd_line,"stat",4)==0) {
			strtok(cmd_line, " ");
			printStats(strtok(NULL, " "));
		}

		else if(strncmp(cmd_line,"cd",2)==0) {	
			//TODO: Handle dumbasses
			strtok(cmd_line, " ");
			changeDirectory(strtok(NULL, " "));
			printf("%d", currentByteAddress);
		}
		else if(strncmp(cmd_line,"ls",2)==0) {
			listFiles();
		}

		else if(strncmp(cmd_line,"read",4)==0) {
			readFile();
		}
		
		else if(strncmp(cmd_line,"quit",4)==0) {
			printf("Quitting.\n");
			break;
		}
		else
			printf("Unrecognized command.\n");


	}

	/* Close the file */
	fclose(filePtr);

	return 0; /* Success */
}

// In: bytesPerSector (uint16_t), sectorsPerCluster (uint8_t), reservedSectorCount (uint16_t), numOfFATs (uint8_t), fatSize (uint32_t)
// Out: none
// Purpose: to print out all relevant information when "info" command is entered
// Notes: 
void printInfo() {
	printf("\nGoing to display info.\n");
	printf("--------------------------\n");
	printf("Bytes Per Sector                => hex: 0x%08x dec: %d\n", thisBootSector.bytesPerSector, thisBootSector.bytesPerSector);
	printf("Sectors Per Cluster             => hex: 0x%08x dec: %d\n", thisBootSector.sectorsPerCluster, thisBootSector.sectorsPerCluster);
	printf("Reserved Sector Count           => hex: 0x%08x dec: %d\n", thisBootSector.reservedSectorCount, thisBootSector.reservedSectorCount);
	printf("Number of FATs                  => hex: 0x%08x dec: %d\n", thisBootSector.numOfFATs, thisBootSector.numOfFATs);
	printf("Size of FATs                    => hex: 0x%08x dec: %d\n", thisBootSector.fatSize, thisBootSector.fatSize);
	printf("Root's First Cluster Number     => hex: 0x%08x dec: %d\n", thisBootSector.rootClusterNum, thisBootSector.rootClusterNum);
	printf("Root Entry Count                => hex: 0x%08x dec: %d\n", thisBootSector.rootEntryCount, thisBootSector.rootEntryCount);
	printf("Total Sectors                   => hex: 0x%08x dec: %d\n", thisBootSector.totalSectors, thisBootSector.totalSectors);
	printf("First Data Sector Sector Number => hex: 0x%08x dec: %d\n", thisBootSector.firstDataSector, thisBootSector.firstDataSector);
	printf("First Data Sector in Bytes      => hex: 0x%08x dec: %d\n\n", thisBootSector.firstDataSector * thisBootSector.bytesPerSector, thisBootSector.firstDataSector * thisBootSector.bytesPerSector);
}

// In:			none
// Out: 		none
// Purpose: 	to print the volume name 
// Notes: 		found in the root directory entry
void printVolumeName() {
	uint64_t byteAddress; 				// the byte address of the first sector of the root cluster
	uint32_t sectorNumber;				// the sector number of the first sector of a cluster
	int sectorByteCounter;				// counter that tracks how many bytes we've read within the current sector
	fileData thisFileData;				// holds the fileData for the current directory entry being looked at

	printf("Volume Names\n");
	printf("---------------------\n");
	
	// 1. determine the first sector of root cluster number
	sectorNumber = getFirstSectorOfCluster(thisBootSector, thisBootSector.rootClusterNum);

	// 2. calculate byte address of this sector number
	byteAddress = sectorNumber * thisBootSector.bytesPerSector;

	sectorByteCounter = 0;
	
	while (sectorByteCounter < thisBootSector.bytesPerSector) {
		// jumps to starting byte address of each directory entry
		thisFileData = getFileData(filePtr, byteAddress);
		thisFileData.fileName[SH_FILE_NAME] = '\0';

		if (isVolumeLabel(thisFileData)) {
			printf("%s\n", thisFileData.fileName);
		}
		
		byteAddress += DIR_ENTRY_SIZE;
		sectorByteCounter += DIR_ENTRY_SIZE;
	}		
}

// In: 			fileName (string)
// Out: 		none
// Purpose:		to print out stats about a give file
// Notes:		
void printStats(char* fileName) {
	fileData thisFileData;
	uint64_t byteAddress;	// the byte address of the given fileName, -1 if that fileName is not in the current directory
	date thisCreateDate;
	time thisCreateTime;
	date thisLastAccessDate;
	date thisWriteDate;
	time thisWriteTime;	

	// check if the file name exists
	byteAddress = checkFileExists(filePtr, fileName, currentByteAddress);
	
	// if the file does not exist, tell the user this
	// if the file exists print out the contents of the associated directory entry
	if (byteAddress == -1) {	
		printf("That file does not exist in this directory.\n");
	}
	else {
		thisFileData 		= getFileData(filePtr, byteAddress);
		thisCreateDate 		= getCreateDate(thisFileData);
		thisCreateTime 		= getCreateTime(thisFileData);
		thisLastAccessDate 	= getLastAccessDate(thisFileData);
		thisWriteDate		= getWriteDate(thisFileData);
		thisWriteTime		= getWriteTime(thisFileData);

		printf("File Name: %s\n", thisFileData.fileName);
		printf("File Attributes: %d\n", thisFileData.fileAttributes);
		printf("Create Date: %02d-%02d-%02d\n", thisCreateDate.month, thisCreateDate.day, thisCreateDate.year); 			
		printf("Create Time: %02d:%02d:%02d\n", thisCreateTime.hours, thisCreateTime.minutes, thisCreateTime.seconds);
		printf("Last Access Date: %02d-%02d-%02d\n", thisLastAccessDate.month, thisLastAccessDate.day, thisLastAccessDate.year); 			
		printf("Last Write Date: %02d-%02d-%02d\n", thisWriteDate.month, thisWriteDate.day, thisWriteDate.year); 			
		printf("Write Time: %02d:%02d:%02d\n", thisWriteTime.hours, thisWriteTime.minutes, thisWriteTime.seconds);
		
	}
}

// In: 			directory (string)
// Out: 		none
// Purpose:
// Notes:
void changeDirectory(char* directory) {
	printf("Going to cd!\n");

	int cwdSet = 0;
	uint64_t byteAddress;	
	fileData thisFileData;

	// set byte address
	byteAddress = currentByteAddress;	

	// get file entry data structure at byte adress
	thisFileData = getFileData(filePtr, byteAddress);

	// while the first character of the fileName in file entry structure is not '\0'
	while (!isEndOfDirectory(thisFileData)) {
			
		// print the fileName
		if (!isEmptyDirectoryEntry(thisFileData)) { 
			if (strncmp(thisFileData.fileName, directory, strlen(directory) - 2) == 0)
			{
				currentByteAddress = thisFileData.firstClusterNumHI;
				currentByteAddress = currentByteAddress << 16;
				currentByteAddress += thisFileData.firstClusterNumLO;
				currentByteAddress = getFirstSectorOfCluster(thisBootSector, currentByteAddress);
				currentByteAddress = convertSectorNumToBytes(thisBootSector, currentByteAddress);	
				cwdSet = 1;
				break;
			}  
		}

		// get new byte address
		byteAddress += 64;	

		// get file entry data structure at new byte address
		thisFileData = getFileData(filePtr, byteAddress);
	}

	if (cwdSet==0)
	{
		printf("Error: The entered directory does not exist\n");
	}	
}

// In: 
// Out: none
// Purpose:
// Notes:
void listFiles() {
	uint64_t byteAddress; 		// holds the address of the directory entry we are accessing
	fileData thisFileData;		// holds the current file data (metadata!)

	printf("Going to ls.\n");
	// set byte address
	byteAddress = currentByteAddress;	

	// get file entry data structure at byte adress
	thisFileData = getFileData(filePtr, byteAddress);

	// while the first character of the fileName in file entry structure is not '\0'
	while (!isEndOfDirectory(thisFileData)) {
			
		// print the fileName
		if (!isEmptyDirectoryEntry(thisFileData) && !isVolumeLabel(thisFileData) && !isLongName(thisFileData)) { 
			printf("%s\n", thisFileData.fileName); 
		}

		// get new byte address
		byteAddress += 32;	

		// get file entry data structure at new byte address
		thisFileData = getFileData(filePtr, byteAddress);
	}
}

// In: 
// Out: none
// Purpose:
// Notes:
void readFile() {
	printf("Going to read!\n");
}

