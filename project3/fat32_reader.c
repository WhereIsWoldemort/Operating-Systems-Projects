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

//////////////////////////////////////////////////////
// MACROS ////////////////////////////////////////////
//////////////////////////////////////////////////////

#define BOOL int
#define TRUE 1
#define FALSE 0

#define MAX_CMD 80

// macros for BPB data structure
#define BPS_OFFSET 		11		// bytes per sector 
#define BPS_SIZE 		2		
#define SPC_OFFSET 		13		// sectors per cluster
#define SPC_SIZE 		1		
#define RSC_OFFSET 		14		// reserved sector count
#define RSC_SIZE 		2		
#define NF_OFFSET 		16		// number of FATs
#define NF_SIZE			1
#define FS_OFFSET		36		// size of FAT
#define FS_SIZE			4
#define RC_OFFSET		44		// root cluster number
#define RC_SIZE			4		
#define REC_OFFSET		17		// root entry count
#define REC_SIZE		2
#define TS_OFFSET		32		// total sectors
#define TS_SIZE			4		

// macros for directory entry data structure
#define DN_OFFSET		0		// directory name
#define DN_SIZE			11
#define DA_OFFSET		11		// directory attributes
#define DA_SIZE			1	
#define DCT_OFFSET		14		// create time
#define DCT_SIZE		2
#define DCD_OFFSET		16		// create date 
#define DCD_SIZE		2
#define DLAD_OFFSET		18		// last access date
#define DLAD_SIZE		2
#define DFCH_OFFSET		20		// first cluster number high order word
#define DFCH_SIZE		2
#define DWT_OFFSET		22		// write time
#define DWT_SIZE		2
#define DWD_OFFSET		24		// write date
#define DWD_SIZE		2
#define DFCL_OFFSET		26		// first cluster number low order word
#define DFCL_SIZE		2
#define DFS_OFFSET		28		// file size
#define DFS_SIZE		4

// general system macros
#define READ_RESOLUTION	1		// the resolution we want to read at in bytes
#define SH_DIRNAME_SIZE	11		// the number of characters in a short directory name
#define DIR_ENTRY_SIZE	32		// the size of each entry within a directory in bytes
#define EPOCH_YEAR 		1980	// the year that all years are calculated from in fat32			

//////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////
//////////////////////////////////////////////////////////
FILE* 		filePtr;				// pointer to the fat32 image file
uint16_t 	bytesPerSector;			// the number of bytes per sector
uint8_t		sectorsPerCluster;		// the number of sectors per cluster
uint16_t 	reservedSectorCount;	// the number of sectors in the reserved region
uint8_t 	numOfFATs; 				// number of FATs
uint32_t 	fatSize;				// the size of each FAT
uint32_t 	rootClusterNum;			// the cluster number of the first cluster of the root
uint16_t	rootEntryCount;			// the number of entries in the root directory (should be 0)
uint32_t 	totalSectors;			// the total number of sectors in this volume
uint32_t 	rootDirectorySectors; 	// number of sectors in the root directory
uint32_t	firstDataSector;		// the first sector of data in the data region of the file system
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

uint32_t getRootDirectorySectors();
uint32_t getFirstDataSector(); 
uint32_t getFirstSectorOfCluster(uint32_t numOfCluster);
uint32_t getThisFATSectorNumber(uint32_t numOfCluster);
uint16_t getThisFATEntryOffset(uint32_t numOfCluster);
uint64_t convertSectorNumToBytes(uint32_t sectorNumber);
fileData getFileData(uint64_t byteAddress);

date convertToDateStruct(uint16_t machineRepresentation);
time convertToTimeStruct(uint16_t machineRepresentation);

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

	/* Parse boot sector and get information */
	
	// bytes per sector
	fseek(filePtr, BPS_OFFSET, SEEK_SET); 
	fread(&bytesPerSector, READ_RESOLUTION, BPS_SIZE, filePtr);
	bytesPerSector = le16toh(bytesPerSector);	

	// sectors per cluster (don't need to switch because SPC_SIZE = 1)
	fseek(filePtr, SPC_OFFSET, SEEK_SET);
	fread(&sectorsPerCluster, READ_RESOLUTION, SPC_SIZE, filePtr);

	// reserved sector count
	fseek(filePtr, RSC_OFFSET, SEEK_SET);
	fread(&reservedSectorCount, READ_RESOLUTION, RSC_SIZE, filePtr);
	reservedSectorCount = le16toh(reservedSectorCount);

	// number of FATs (don't need to switch because NF_SIZE = 1)
	fseek(filePtr, NF_OFFSET, SEEK_SET);
	fread(&numOfFATs, READ_RESOLUTION, NF_SIZE, filePtr);	

	// size of FAT
	fseek(filePtr, FS_OFFSET, SEEK_SET);
	fread(&fatSize, READ_RESOLUTION, FS_SIZE, filePtr);		
	fatSize = le32toh(fatSize);	

	// root cluster number
	fseek(filePtr, RC_OFFSET, SEEK_SET);
	fread(&rootClusterNum, READ_RESOLUTION, RC_SIZE, filePtr);
	rootClusterNum = le32toh(rootClusterNum);

	// total number of sectors
	fseek(filePtr, TS_OFFSET, SEEK_SET);
	fread(&totalSectors, READ_RESOLUTION, TS_SIZE, filePtr);
	totalSectors = le32toh(totalSectors);

	// root entry count
	fseek(filePtr, REC_OFFSET, SEEK_SET);
	fread(&rootEntryCount, READ_RESOLUTION, REC_SIZE, filePtr);
	rootEntryCount = le16toh(rootEntryCount);

	rootDirectorySectors = getRootDirectorySectors();
	firstDataSector = getFirstDataSector();

	fatSectorNumber = getThisFATSectorNumber(129);
	fatEntryOffset = getThisFATEntryOffset(129);
	printf("Cluster %d is located in sector %d of the FAT and has an offset of %d within that sector.\n", 129, fatSectorNumber, fatEntryOffset);
	
	/* Get root directory address */
	rootFirstSectorNum = getFirstSectorOfCluster(rootClusterNum);
	currentDirectory = convertSectorNumToBytes(rootFirstSectorNum);
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

	/* Close the file/
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
	printf("Bytes Per Sector                => hex: 0x%08x dec: %d\n", bytesPerSector, bytesPerSector);
	printf("Sectors Per Cluster             => hex: 0x%08x dec: %d\n", sectorsPerCluster, sectorsPerCluster);
	printf("Reserved Sector Count           => hex: 0x%08x dec: %d\n", reservedSectorCount, reservedSectorCount);
	printf("Number of FATs                  => hex: 0x%08x dec: %d\n", numOfFATs, numOfFATs);
	printf("Size of FATs                    => hex: 0x%08x dec: %d\n", fatSize, fatSize);
	printf("Root's First Cluster Number     => hex: 0x%08x dec: %d\n", rootClusterNum, rootClusterNum);
	printf("Root Entry Count                => hex: 0x%08x dec: %d\n", rootEntryCount, rootEntryCount);
	printf("Total Sectors                   => hex: 0x%08x dec: %d\n", totalSectors, totalSectors);
	printf("First Data Sector Sector Number => hex: 0x%08x dec: %d\n", firstDataSector, firstDataSector);
	printf("First Data Sector in Bytes      => hex: 0x%08x dec: %d\n\n", firstDataSector * bytesPerSector, firstDataSector * bytesPerSector);
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
	sectorNumber = getFirstSectorOfCluster(rootClusterNum);

	// 2. calculate byte address of this sector number
	byteAddress = sectorNumber * bytesPerSector;

	sectorByteCounter = 0;
	
	while (sectorByteCounter < bytesPerSector) {
		// jumps to starting byte address of each directory entry
		thisFileData = getFileData(byteAddress);
		thisFileData.fileName[SH_DIRNAME_SIZE] = '\0';

		if ((thisFileData.fileAttributes & 8) == 8) {
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
	thisFileData = getFileData(byteAddress);

	// while the first character of the fileName in file entry structure is not '\0'
	while (thisFileData.fileName[0] != '\0') {
			
		// print the fileName
		if (thisFileData.fileName[0] != -27) { 
			//TODO: Why is there a magic humber?!?!?!?!?!
			if (strncmp(thisFileData.fileName, directory, strlen(directory) - 2) == 0)
			{
				currentDirectory = thisFileData.firstClusterNumHI;
				currentDirectory = currentDirectory << 16;
				currentDirectory += thisFileData.firstClusterNumLO;
				currentDirectory = getFirstSectorOfCluster(currentDirectory);
				currentDirectory = convertSectorNumToBytes(currentDirectory);	
				cwdSet = 1;
				break;
			}  
		}

		// get new byte address
		byteAddress += 64;	

		// get file entry data structure at new byte address
		thisFileData = getFileData(byteAddress);
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
	thisFileData = getFileData(byteAddress);

	// while the first character of the fileName in file entry structure is not '\0'
	while (thisFileData.fileName[0] != '\0') {
			
		// print the fileName
		if (thisFileData.fileName[0] != -27) { 
			//TODO: Why is there a magic number??!?!?!?!?
			printf("%s\n", thisFileData.fileName); 
		}

		// get new byte address
		byteAddress += 64;	

		// get file entry data structure at new byte address
		thisFileData = getFileData(byteAddress);
	}
}

// In: 
// Out: none
// Purpose:
// Notes:
void readFile() {
	printf("Going to read!\n");
}

// In: 
// Out: 
// Purpose:
// Notes:
uint32_t getRootDirectorySectors() {
	return (((rootEntryCount * 32) + (bytesPerSector - 1)) / bytesPerSector); 
}

// In: 
// Out: 
// Purpose: to get the first sectors of the data region of the file system
// Notes:
uint32_t getFirstDataSector() {
	return (reservedSectorCount + (numOfFATs * fatSize) + rootDirectorySectors);	
} 

// In: 
// Out: 
// Purpose:
// Notes:
uint32_t getFirstSectorOfCluster(uint32_t numOfCluster) {
	return (((numOfCluster - 2) * sectorsPerCluster) + firstDataSector);
}

// In: 
// Out: 
// Purpose: to get the sector number of the FAT entry for this cluster
// Notes: round down becaues of int division
uint32_t getThisFATSectorNumber(uint32_t numOfCluster) {
	uint64_t fatOffset;
	fatOffset = numOfCluster * 4;
	return (reservedSectorCount + (fatOffset / bytesPerSector)); 
}

// In: 
// Out: 
// Purpose: to get the byte offset within the sector number when determining the byte address of the FAT entry for this cluster
// Notes:
uint16_t getThisFATEntryOffset(uint32_t numOfCluster) {
	uint64_t fatOffset;
	fatOffset = numOfCluster * 4;
	return (fatOffset % bytesPerSector);
}

// In: 		byteAddress (uint64_t)
// Out: 	thisFileData (fileData (struct))
// Purpose: to place all file metadata based on a byte address in a struct and return it
// Note: 	handles file metadata blocks that are 32 bits (4 bytes) in size
fileData getFileData(uint64_t byteAddress) {
	fileData thisFileData;
	
	// 1. jump to where the file data is
	fseek(filePtr, byteAddress, SEEK_SET);

	// 2. get the directory name
	fread(&(thisFileData.fileName), READ_RESOLUTION, DN_SIZE, filePtr);
	thisFileData.fileName[SH_DIRNAME_SIZE] = '\0';
	
	// 3. get the file attributes
	fread(&(thisFileData.fileAttributes), READ_RESOLUTION, DA_SIZE, filePtr);

	// 4. get the create time
	fseek(filePtr, DCT_OFFSET - (DA_OFFSET + DA_SIZE), SEEK_CUR);
	fread(&(thisFileData.createTime), READ_RESOLUTION, DCT_SIZE, filePtr);
	thisFileData.createTime = le16toh(thisFileData.createTime);

	// 5. get the create date
	fread(&(thisFileData.createDate), READ_RESOLUTION, DCD_SIZE, filePtr);
	thisFileData.createDate = le16toh(thisFileData.createDate);
	
	// 6. get the last access date
	fread(&(thisFileData.lastAccessDate), READ_RESOLUTION, DLAD_SIZE, filePtr);
	thisFileData.lastAccessDate = le16toh(thisFileData.lastAccessDate);

	// 7. get the first cluster number high order word
	fread(&(thisFileData.firstClusterNumHI), READ_RESOLUTION, DFCH_SIZE, filePtr);
	thisFileData.firstClusterNumHI = le16toh(thisFileData.firstClusterNumHI);

	// 8. get the last write time
	fread(&(thisFileData.writeTime), READ_RESOLUTION, DWT_SIZE, filePtr);
	thisFileData.writeTime = le16toh(thisFileData.writeTime);

	// 9. get the last write date
	fread(&(thisFileData.writeDate), READ_RESOLUTION, DWD_SIZE, filePtr);
	thisFileData.writeDate = le16toh(thisFileData.writeDate);

	// 10. get the first cluster number low order word
	fread(&(thisFileData.firstClusterNumLO), READ_RESOLUTION, DFCL_SIZE, filePtr);
	thisFileData.firstClusterNumLO = le16toh(thisFileData.firstClusterNumLO);

	// 11. get the file size
	fread(&(thisFileData.fileSize), READ_RESOLUTION, DFS_SIZE, filePtr);
	thisFileData.fileSize = le32toh(thisFileData.fileSize);

	return thisFileData;
}

// In: 			machineRepresentation (uint16_t)
// Out: 		thisDate (date (struct))
// Purpose: 	to convert the machine representation of the date into a human readable date
// Notes:		specific bits are extracted through shifting and using & for masking
date convertToDateStruct(uint16_t machineRepresentation) {
	date thisDate; 
	uint16_t mask;

	// 1. make a bit mask for bits 5-15
	mask = ~(~0 << 5);

	// 2. take bitwise AND of this and the machine represenation
	thisDate.day = mask & machineRepresentation;

	// 3. shift 5 bits to the right to put month in the lowest order
	machineRepresentation = machineRepresentation >> 5;

	// 4. make a bit mask for bits 4-15
	mask = ~(~0 << 4);

	// 5. take bitwise AND of this and the machine represenation
	thisDate.month = mask & machineRepresentation;

	// 6. shift 4 bits to the right to put year in lowest order
	machineRepresentation = machineRepresentation >> 4;

	// 7. make a bit mask for 7-15 
	mask = ~(~0 << 7);

	// 8. take bitwise AND of this and the machine representation
	thisDate.year = mask & machineRepresentation;

	// 9. add epoch to the calculated year
	thisDate.year += EPOCH_YEAR;

	return thisDate;
}

// In: 		machineRepresentation (uint16_t)
// Out:		thisTime (time (struct))
// Purpose: to convert the machine representation of a time to a human-readable version
// Notes:	same logic as convertToDateStruct
time convertToTimeStruct(uint16_t machineRepresentation) {
	time thisTime;
	uint16_t mask;

	// 1. make a bit mask for bits 5-15
	mask = ~(~0 << 5);
	
	// 2. take a bitwise AND of this and the machine representation
	thisTime.seconds = mask & machineRepresentation;

	// 3. multiply seconds by 2 (because granularity is 2 seconds)
	thisTime.seconds *= 2;

	// 4. shift 5 bits to the right to put minutes in lowest order
	machineRepresentation = machineRepresentation >> 5;

	// 5. make a bit mask for 6-15
	mask = ~(~0 << 6);

	// 6. take a bitwise AND of this and the machine representation
	thisTime.minutes = mask & machineRepresentation;

	// 7. shift 6 bits to the right to put hours in the lowest order
	machineRepresentation = machineRepresentation >> 6;

	// 8. make a bit mask for 5-15
	mask = ~(~0 << 5);

	// 9. take a bitwise AND of this and the machine representation
	thisTime.hours = mask & machineRepresentation;

	return thisTime;
}

uint64_t convertSectorNumToBytes(uint32_t sectorNumber) {
	return sectorNumber * bytesPerSector;
}

 
