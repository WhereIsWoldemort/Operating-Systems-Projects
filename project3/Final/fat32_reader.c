/***********************************************************
 * Name of program: fat32_reader.c
 * Authors:			Ethan Morisette and Kaleb Luse
 * Description:		a shell interface to a fat32 file system image
 **********************************************************/

/* These are the included libraries.  You may need to add more. */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
#include "fat_entries.h"
//#include "format.h"

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
uint64_t    parentDirectoryAddress = -1;

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
		}
		else if(strncmp(cmd_line,"ls",2)==0) {
			listFiles();
		}

		else if(strncmp(cmd_line,"read",4)==0) {
			strtok(cmd_line, " ");
			readFile(strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "));
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
	char* ext;
	char* name;
	char toPrint[12];	
	int i;

	// check if the file name exists
	byteAddress = checkFileExists(filePtr, thisBootSector, fileName, convertBytesToClusterNum(thisBootSector, currentByteAddress));
	
	// if the file does not exist, tell the user this
	// if the file exists print out the contents of the associated directory entry
	if (byteAddress == -1) {	
		printf("That file does not exist in this directory.\n");
	} else {
		thisFileData 		= getFileData(filePtr, byteAddress);

		name = strtok(thisFileData.fileName, " ");

		if (isFile(thisFileData))
		{
			ext = strtok(NULL, " ");
		}

		strcpy(toPrint, name);
		if (isFile(thisFileData))
		{
			strcat(toPrint, ".");
			strcat(toPrint, ext);
		}

		for (i = 0; i < 12; i++)
		{
			toPrint[i] = tolower(toPrint[i]);
		}

		printf("File Name: %s\n", toPrint);
		printf("File Size: %d\n", thisFileData.fileSize);

		switch (thisFileData.fileAttributes) {
			case 1:
				printf("File Attribute: Read Only File\n");
				break;
			
			case 2:
				printf("File Attribute: Hidden File\n");
				break;

			case 4:
				printf("File Attribute: System File\n");
				break;

			case 8:
				printf("File Attribute: Volume\n");
				break;

			case 16:
				printf("File Attribute: Directory\n");
				break;

			case 32:
				printf("File Attribute: Archive File\n");
				break;
		}	

		uint32_t firstClusterEntry;

		//Get the high word from the file and shift it left 16 bits
		firstClusterEntry = thisFileData.firstClusterNumHI;
		firstClusterEntry = firstClusterEntry << 16;

		//Get the low word from the file and add it to the first cluster entry
		firstClusterEntry += thisFileData.firstClusterNumLO;

		printf("Next Cluster Number: 0x%x\n", firstClusterEntry);
	}
}

// In: 			directory (string)
// Out: 		none
// Purpose:
// Notes:
void changeDirectory(char* directory) {
	uint64_t cwdSet = 0;
	uint64_t byteAddress;	
	uint32_t currentClusterNumber;
	fileData thisFileData;


	cwdSet = checkFileExists(filePtr, thisBootSector, directory, convertBytesToClusterNum(thisBootSector, currentByteAddress));
	if (cwdSet==-1)
	{
		printf("Error: The entered value does not exist\n");
	}
	else
	{	
		thisFileData = getFileData(filePtr, cwdSet);
		if (isDirectory(thisFileData))
		{
			currentByteAddress = cwdSet;
			currentByteAddress = thisFileData.firstClusterNumHI;
			currentByteAddress = currentByteAddress << 16;
			currentByteAddress += thisFileData.firstClusterNumLO;
			currentByteAddress = getFirstSectorOfCluster(thisBootSector, currentByteAddress);
			currentByteAddress = convertSectorNumToBytes(thisBootSector, currentByteAddress);
			printf("%x\n", currentByteAddress);
		}
		else
		{
			printf("Error: The entered value is not a directory\n");
		}			
	}
}

// In: 
// Out: none
// Purpose:
// Notes:
void listFiles() {
	uint64_t byteAddress; 		// holds the address of the directory entry we are accessing
	fileData thisFileData;		// holds the current file data (metadata!)
	char toPrint[SH_DIRNAME_SIZE + 1];
	char* ext;
	char* name;
	int i = 0;
	uint32_t currentClusterNumber;
	uint32_t firstSectorOfCluster;
	uint64_t sectorNumberInBytes;
	FATEntry thisFATEntry;

	uint32_t sectorNumber;

	currentClusterNumber = convertBytesToClusterNum(thisBootSector, currentByteAddress);

	while (currentClusterNumber < EOC) {
		firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, currentClusterNumber);		
	
		for (sectorNumber = firstSectorOfCluster; sectorNumber < (firstSectorOfCluster + thisBootSector.sectorsPerCluster); sectorNumber++) {
			sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, sectorNumber);	
			
			for	(byteAddress = sectorNumberInBytes; byteAddress < (sectorNumberInBytes + thisBootSector.bytesPerSector); byteAddress += DIR_ENTRY_SIZE) {
				thisFileData = getFileData(filePtr, byteAddress);	

				if (isEndOfDirectory(thisFileData)) {
					printf("\n");
					return;
				}

				// if this is not a volume lable, long name, or deleted entry
				if (!isVolumeLabel(thisFileData) && !isLongName(thisFileData) && !isEmptyDirectoryEntry(thisFileData)) {
						name = strtok(thisFileData.fileName, " ");
						if (isFile(thisFileData))
						{
							ext = strtok(NULL, " ");
						}

						strcpy(toPrint, name);
						if (isFile(thisFileData))
						{
							strcat(toPrint, ".");
							strcat(toPrint, ext);
						}

						for (i = 0; i < SH_DIRNAME_SIZE + 1; i++)
						{
							toPrint[i] = tolower(toPrint[i]);
						}

						printf("%s   ", toPrint);	  	
				}
			}
		}

		// determine the next currentClusterNumber
		thisFATEntry = getFATEntry(filePtr, thisBootSector, currentClusterNumber);
		currentClusterNumber = thisFATEntry.nextClusterNumber;
	}
	printf("\n");
}

// In: 
// Out: none
// Purpose:
// Notes:
void readFile(char* to, char* from, char* file) {
	fileData thisFileData;  		//Represents the header for the file to read
	uint64_t fileLoc;	 			//Represents the location of the file
	uint32_t currentClusterNumber;	//Represents the first cluster location of the data from the file to read
	char 	 myChar;				//Represents the character to print out
	uint32_t sectorNumber;			//Represents the current sector number within the loop
	uint64_t byteAddress;			//Represents the current byte address within the loop
	uint32_t firstSectorOfCluster;  //Represents the first sector of data for the file
	uint64_t sectorNumberInBytes;	//Represents the current sector num in bytes
	uint64_t startingByteAddress;  	//Represents the byte address of the starting cluster
	FATEntry thisFATEntry;			//Represents the FAT entry
	char* 	 ptr;
	uint32_t fromOffset;
	uint32_t toOffset;


	printf("%s", to);
	printf("%s", from);

	fromOffset = strtoul(from, &ptr, 10);
	toOffset = strtoul(to, &ptr, 10);

	//Check to see if the file passed in exists
	fileLoc = checkFileExists(filePtr, thisBootSector, file, convertBytesToClusterNum(thisBootSector, currentByteAddress));

	//If the file doesn't exist yell at Dr. Diesburg
	if (fileLoc == -1)
	{
		printf("Silly Dr. Diesburg, that file doesn't exist!\n");
		return;
	}

	//Read in the file header
	thisFileData = getFileData(filePtr, fileLoc);

	//Find the address of the data
	currentClusterNumber = getFirstClusterOfEntry(thisBootSector, thisFileData);

	//Convert the current cluster number 
	startingByteAddress = convertSectorNumToBytes(thisBootSector, getFirstSectorOfCluster(thisBootSector, currentClusterNumber));


	printf("%x\n", (fromOffset));
	printf("%x\n", (toOffset));
	printf("%x\n", (startingByteAddress + fromOffset));
	printf("%x\n", (startingByteAddress + toOffset));

	while (currentClusterNumber < EOC) {
		firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, currentClusterNumber);		
	
		for (sectorNumber = firstSectorOfCluster; sectorNumber < (firstSectorOfCluster + thisBootSector.sectorsPerCluster); sectorNumber++) {
			sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, sectorNumber);	
			
			//We're sorry
			for	(byteAddress = sectorNumberInBytes; byteAddress < (sectorNumberInBytes + thisBootSector.bytesPerSector); byteAddress += 1) {
				if (byteAddress >= (startingByteAddress + toOffset)) {
					printf("\n");
					return;
				}

				if (byteAddress >= (startingByteAddress + fromOffset))
				{
					fseek(filePtr, byteAddress, SEEK_SET);
					fread(&(myChar), READ_RESOLUTION, 1, filePtr);
					printf("%c", myChar);
				}
			}
		}

		// determine the next currentClusterNumber
		thisFATEntry = getFATEntry(filePtr, thisBootSector, currentClusterNumber);
		currentClusterNumber = thisFATEntry.nextClusterNumber;
	}
	printf("\n");
}

