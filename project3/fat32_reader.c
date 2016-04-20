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
#define DIR_ENTRY_SIZE	32		// the size of each entry within a directory in bytes

//////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////
//////////////////////////////////////////////////////////
FILE* 		filePtr;				// pointer to the fat32 image file
bootSector  thisBootSector;
uint64_t 	currentDirectory;		// stores the byte address of the current directory

//////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES ///////////////////////////////////
//////////////////////////////////////////////////////////
void printInfo();
void printVolumeName();
void printStats();
void listFiles();
void changeDirectory();
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
	currentDirectory = convertSectorNumToBytes(thisBootSector, rootFirstSectorNum);
	printf("Root addr is 0x%x\n", currentDirectory);


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
			printStats();
		}

		else if(strncmp(cmd_line,"cd",2)==0) {	
			//TODO: Handle dumbasses
			strtok(cmd_line, " ");
			changeDirectory(strtok(NULL, " "));
			printf("%d", currentDirectory);
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

// In:		rootCluster (uint32_t), bytesPerSector (uint16_t), sectorsPerCluster (uint8_t), firstDataSector (uint32_t)
// Out: 	none
// Purpose: to print the volume name
// Notes: 	found in the root directory entry
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

// In: 
// Out: 	none
// Purpose:
// Notes:
void printStats(/* file name */) {
	printf("Going to stat!\n");
	// check if the file name exists
	
	// if the file exists print out the contents of the associated directory entry

	// if the file does not exist, tell the user this
}

// In: 
// Out: 	none
// Purpose:
// Notes:
void changeDirectory(char* directory ) {
	printf("Going to cd!\n");

	int cwdSet = 0;
	uint64_t byteAddress;	
	fileData thisFileData;

	// set byte address
	byteAddress = currentDirectory;	

	// get file entry data structure at byte adress
	thisFileData = getFileData(filePtr, byteAddress);

	// while the first character of the fileName in file entry structure is not '\0'
	while (!isEndOfDirectory(thisFileData)) {
			
		// print the fileName
		if (!isEmptyDirectoryEntry(thisFileData)) { 
			if (strncmp(thisFileData.fileName, directory, strlen(directory) - 2) == 0)
			{
				currentDirectory = thisFileData.firstClusterNumHI;
				currentDirectory = currentDirectory << 16;
				currentDirectory += thisFileData.firstClusterNumLO;
				currentDirectory = getFirstSectorOfCluster(thisBootSector, currentDirectory);
				currentDirectory = convertSectorNumToBytes(thisBootSector, currentDirectory);	
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
	byteAddress = currentDirectory;	

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

