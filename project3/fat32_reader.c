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
#include "color.h"
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
FILE* 		filePtr;						// pointer to the fat32 image file
bootSector  thisBootSector;					// a struct of all system information
uint64_t 	currentByteAddress;				// stores the byte address of the current directory
uint64_t    parentDirectoryAddress = -1;	// the address of the parent directory to the directory we are currently in

//////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES ///////////////////////////////////
//////////////////////////////////////////////////////////
void printInfo();
void printVolumeName();
void printStats(char* fileName);
void listFiles(char* directory);
void changeDirectory(char* directory);
void readFile();
void printHelp();
void replaceNewLine(char* string);

/* This is the main function of your project, and it will be run
 * first before all other functions.
 */
int main(int argc, char *argv[])
{
	char 		cmd_line[MAX_CMD];		// command line container for our file system shell
	uint32_t 	rootFirstSectorNum;		// the first sector of the root directory

	/* Parse args and open our image file */
	if (argc != 2) {
		printf("\n");
		printColor("red", "You entered an incorrect amount of arguments for this program. <USAGE: ./fat32_reader fat32.img>\n", stdout);
		printf("\n");
		return -1;
	}

	if ((filePtr = fopen(argv[1], "rb")) == 0) {
		perror(argv[1]);
		return -1;
	}

	printf("\n");
	printColorTemplate("panther", "  /\\                                      /\\ \n", stdout);
	printColorTemplate("panther", " /  \\                                    /  \\ \n", stdout);
	printColorTemplate("panther", "/    \\                                  /    \\ \n", stdout);
	printColorTemplate("panther", "|     \\	                               /      |   \n", stdout);
	printColorTemplate("panther", "|      \\                              /       |\n", stdout);
	printColorTemplate("panther", "|       \\     =================      /        | \n", stdout);
	printColorTemplate("panther", "|         ===========================         |\n", stdout);
	printColorTemplate("panther", "|        =============================        | \n", stdout);
	printColorTemplate("panther", "|      =================================      |  \n", stdout);
	printColorTemplate("panther", "|     ===================================     | \n", stdout);
	printColorTemplate("panther", "|    =====================================    | \n", stdout);
	printColorTemplate("panther", "|   =======================================   | \n", stdout);
	printColorTemplate("panther", "|  =========================================  |\n", stdout);
	printColorTemplate("panther", "| =========================================== | \n", stdout);
	printColorTemplate("panther", "|=============================================| \n", stdout);
	printColorTemplate("panther", "===============================================\n", stdout);
	printColorTemplate("panther", "|||||                                     |||||\n", stdout); 
	printColorTemplate("panther", "|||||          PANTHER PROMPT 1.0         |||||\n", stdout); 
	printColorTemplate("panther", "|||||                                     |||||\n", stdout); 
	printColorTemplate("panther", "===============================================\n", stdout);
	printColorTemplate("panther", " =============================================\n", stdout);
	printColorTemplate("panther", "  ===========================================\n", stdout);
	printColorTemplate("panther", "   =========================================\n", stdout);
	printColorTemplate("panther", "    =======================================\n", stdout);
	printColorTemplate("panther", "     =====================================\n", stdout);
	printColorTemplate("panther", "      ===================================\n", stdout);
	printColorTemplate("panther", "       =================================\n", stdout);
	printColorTemplate("panther", "         =============================\n", stdout);
	printColorTemplate("panther", "           =========================\n", stdout);
	printColorTemplate("panther", "               =================\n", stdout);
	printf("\n");

	thisBootSector = setUpBootSector(filePtr);

	/* Get root directory address */
	rootFirstSectorNum = getFirstSectorOfCluster(thisBootSector, thisBootSector.rootClusterNum);
	currentByteAddress = convertSectorNumToBytes(thisBootSector, rootFirstSectorNum);
	//printf("Root addr is hex: 0x%x, dec: %d\n", currentByteAddress, currentByteAddress);

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
			strtok(cmd_line, " ");
			changeDirectory(strtok(NULL, " "));
		}
		else if(strncmp(cmd_line,"ls",2)==0) {
			strtok(cmd_line, " ");
			listFiles(strtok(NULL, " "));
		}
		else if(strncmp(cmd_line,"read",4)==0) {
			strtok(cmd_line, " ");
			readFile(strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "));
		}
		else if(strncmp(cmd_line,"quit",4)==0) {
			printf("\n");
			printColorTemplate("rainbow", "Quitting.\n", stdout);
			printf("\n");
			break;
		}
		else if (strncmp(cmd_line, "help", 4) == 0) {
			printHelp();
		}
		else {
			printColor("red", "Unrecognized command.\n", stdout);
		}

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
	printColorTemplate("panther", "\n                          Going to display info.\n", stdout);
	printColorTemplate("panther", "--------------------------------------------------------------------------\n", stdout);
	printf("Bytes Per Sector                => hex: 0x%08x dec: %d\n", thisBootSector.bytesPerSector, thisBootSector.bytesPerSector);
	printf("Sectors Per Cluster             => hex: 0x%08x dec: %d\n", thisBootSector.sectorsPerCluster, thisBootSector.sectorsPerCluster);
	printf("Reserved Sector Count           => hex: 0x%08x dec: %d\n", thisBootSector.reservedSectorCount, thisBootSector.reservedSectorCount);
	printf("Number of FATs                  => hex: 0x%08x dec: %d\n", thisBootSector.numOfFATs, thisBootSector.numOfFATs);
	printf("Size of FATs                    => hex: 0x%08x dec: %d\n", thisBootSector.fatSize, thisBootSector.fatSize);
	printf("Root's First Cluster Number     => hex: 0x%08x dec: %d\n", thisBootSector.rootClusterNum, thisBootSector.rootClusterNum);
	printf("Root Entry Count                => hex: 0x%08x dec: %d\n", thisBootSector.rootEntryCount, thisBootSector.rootEntryCount);
	printf("Total Sectors                   => hex: 0x%08x dec: %d\n", thisBootSector.totalSectors, thisBootSector.totalSectors);
	printf("First Data Sector Sector Number => hex: 0x%08x dec: %d\n", thisBootSector.firstDataSector, thisBootSector.firstDataSector);
	printf("First Data Sector in Bytes      => hex: 0x%08x dec: %d\n", thisBootSector.firstDataSector * thisBootSector.bytesPerSector, thisBootSector.firstDataSector * thisBootSector.bytesPerSector);
	printColorTemplate("panther", "--------------------------------------------------------------------------\n", stdout);
	printf("\n");
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
	BOOL volumeFound;					// flag to indicate if colume was found

	printf("\n");
	printColorTemplate("panther", "           Volume Name(s)\n", stdout);
	printColorTemplate("panther", "---------------------------------\n", stdout);
	
	// 1. determine the first sector of root cluster number
	sectorNumber = getFirstSectorOfCluster(thisBootSector, thisBootSector.rootClusterNum);

	// 2. calculate byte address of this sector number
	byteAddress = sectorNumber * thisBootSector.bytesPerSector;

	sectorByteCounter = 0;
	volumeFound = FALSE;	

	while (sectorByteCounter < thisBootSector.bytesPerSector) {
		// jumps to starting byte address of each directory entry
		thisFileData = getFileData(filePtr, byteAddress);
		thisFileData.fileName[SH_FILE_NAME] = '\0';

		if (isVolumeLabel(thisFileData)) {
			printf("            %s\n", thisFileData.fileName);
			volumeFound = TRUE;
		}
		
		byteAddress += DIR_ENTRY_SIZE;
		sectorByteCounter += DIR_ENTRY_SIZE;
	}

	// if after all this looping, no volume name is found, print error
	if (!volumeFound) {
		printColor("red", "Error: no volume name found\n", stdout);
	}	

	printColorTemplate("panther", "---------------------------------\n", stdout);
	printf("\n");		
}

// In: 			fileName (string)
// Out: 		none
// Purpose:		to print out stats about a give file
// Notes:		
void printStats(char* fileName) {
	fileData thisFileData;	// stores the info in the current directory we are looking
	uint64_t byteAddress;	// the byte address of the given fileName, -1 if that fileName is not in the current directory
	char* ext;
	char* name;
	char toPrint[12];		
	int i;					// looping variable

	if (fileName == '\0') {
		printColor("red", "ERROR: incorrect usage; USAGE: <stat fileName>\n", stdout);
		return;
	}

	replaceNewLine(fileName);

	// check if the file name exists
	byteAddress = checkFileExists(filePtr, thisBootSector, fileName, convertBytesToClusterNum(thisBootSector, currentByteAddress));
	
	// if the file does not exist, tell the user this
	// if the file exists print out the contents of the associated directory entry
	
	if (byteAddress == -1) {	
		printf("\n");
		printColor("red", "ERROR: That file does not exist in this directory.\n", stdout);
		printf("\n");
	} 
	else {
		thisFileData = getFileData(filePtr, byteAddress);

		// this part of our code assumes space are between the file name and extension
		// grab the file name
		name = strtok(thisFileData.fileName, " ");

		// if this directory entry is for a file that means it has an extension
		// so grab the extension
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

		printf("\n");
		printColorTemplate("panther", "Printing stats...\n", stdout);
		printColorTemplate("panther", "-------------------------\n", stdout);
		printf("File Name:           %s\n", toPrint);
		printf("File Size:           %d\n", thisFileData.fileSize);

		switch (thisFileData.fileAttributes) {
			case 1:
				printf("File Attribute:      Read Only File\n");
				break;
			
			case 2:
				printf("File Attribute:      Hidden File\n");
				break;

			case 4:
				printf("File Attribute:      System File\n");
				break;

			case 8:
				printf("File Attribute:      Volume\n");
				break;

			case 16:
				printf("File Attribute:      Directory\n");
				break;

			case 32:
				printf("File Attribute:      Archive File\n");
				break;
		}	

		uint32_t firstClusterEntry;

		//Get the high word from the file and shift it left 16 bits
		firstClusterEntry = thisFileData.firstClusterNumHI;
		firstClusterEntry = firstClusterEntry << 16;

		//Get the low word from the file and add it to the first cluster entry
		firstClusterEntry += thisFileData.firstClusterNumLO;

		printf("Next Cluster Number: 0x%04x\n", firstClusterEntry);
		
		printColorTemplate("panther", "-------------------------\n", stdout);
		printf("\n");
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

	// if the user doesn't enter anything, they go back to root 
	if (directory == '\0') {
		currentByteAddress = 1049600;
		return;
	}
		
	replaceNewLine(directory);
	cwdSet = checkFileExists(filePtr, thisBootSector, directory, convertBytesToClusterNum(thisBootSector, currentByteAddress));
	if (cwdSet==-1)
	{
		printf("\n");
		printColor("red", "Error: The entered value does not exist\n", stdout);
		printf("\n");
	}
	else
	{	
		thisFileData = getFileData(filePtr, cwdSet);
		
		if ((strncmp(directory, "..", 2) == 0) && parentDirectoryAddress != -1) {
			currentByteAddress = parentDirectoryAddress;			
		}

		else if (isDirectory(thisFileData))
		{
			// store our current address before we move
			parentDirectoryAddress = currentByteAddress;
			currentByteAddress = cwdSet;
			currentByteAddress = thisFileData.firstClusterNumHI;
			currentByteAddress = currentByteAddress << 16;
			currentByteAddress += thisFileData.firstClusterNumLO;
			currentByteAddress = getFirstSectorOfCluster(thisBootSector, currentByteAddress);
			currentByteAddress = convertSectorNumToBytes(thisBootSector, currentByteAddress);
			//Handle Root issues
			if (currentByteAddress == 1048576)
			{
				currentByteAddress = 1049600;
				parentDirectoryAddress == -1;
			}
		}
		else
		{
			printf("\n");
			printColor("red", "Error: The entered value is not a directory\n", stdout);
			printf("\n");
		}			
	}
}

// In: 			directory (string)
// Out: 		none
// Purpose:		to list files based on a directory name 
// Notes:		if no directory name is provided then list files in the current directory
void listFiles(char* directory) {
	uint64_t byteAddress; 				// holds the address of the directory entry we are accessing
	fileData thisFileData;				// holds the current file data (metadata!)
	char toPrint[SH_DIRNAME_SIZE + 1];
	char* ext;
	char* name;
	int i = 0;
	uint32_t currentClusterNumber;
	uint32_t firstSectorOfCluster;
	uint64_t sectorNumberInBytes;
	FATEntry thisFATEntry;
	int cwdSet;
	char changeBack[3] = "..";
	uint64_t hereByteAddress;

	uint32_t byteAddressOfSearch;

	uint32_t sectorNumber;

	if(directory != '\0')
	{		
		replaceNewLine(directory);

		cwdSet = checkFileExists(filePtr, thisBootSector, directory, convertBytesToClusterNum(thisBootSector, currentByteAddress)); 
		hereByteAddress = currentByteAddress;

		if (cwdSet==-1)
		{
			printf("\n");
			printColor("red", "Error: The entered value does not exist\n", stdout);
			printf("\n");
			return;
		}
		else
		{	
			thisFileData = getFileData(filePtr, cwdSet);
			if (isDirectory(thisFileData))
			{
				if (strncmp(directory, "..", 2) == 0)
				{
					printf("\n");
					changeDirectory(changeBack);
					listFiles('\0');
					

					currentByteAddress = hereByteAddress;

					printf("\n");
					return;
				}
				else
				{
					changeDirectory(directory);
				}
			}
			else
			{
				printf("\n");
				printColor("red", "Error: The entered value is not a directory\n", stdout);
				printf("\n");
				return;
			}	
		}
	}

	currentClusterNumber = convertBytesToClusterNum(thisBootSector, currentByteAddress);

	while (currentClusterNumber < EOC) {
		firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, currentClusterNumber);		
	
		for (sectorNumber = firstSectorOfCluster; sectorNumber < (firstSectorOfCluster + thisBootSector.sectorsPerCluster); sectorNumber++) {
			sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, sectorNumber);	
			
			for	(byteAddress = sectorNumberInBytes; byteAddress < (sectorNumberInBytes + thisBootSector.bytesPerSector); byteAddress += DIR_ENTRY_SIZE) {
				thisFileData = getFileData(filePtr, byteAddress);	

				if (isEndOfDirectory(thisFileData)) {
					printf("\n");

					if (directory != '\0')
					{
						changeDirectory(changeBack);
					}
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

	// if the user doesn't enter anything, then print an erro	
	if ((file == '\0') || (to == '\0') || (from == '\0')) {
		printColor("red", "Error: incorrect usage; USAGE: <read fileName 0 10>\n", stdout);
		return;
	}

	fromOffset = strtoul(from, &ptr, 10);
	toOffset = strtoul(to, &ptr, 10);

	//Check to see if the file passed in exists
	fileLoc = checkFileExists(filePtr, thisBootSector, file, convertBytesToClusterNum(thisBootSector, currentByteAddress));

	//If the file doesn't exist yell at Dr. Diesburg
	if (fileLoc == -1)
	{
		printf("\n");
		printColor("red", "Silly Dr. Diesburg, that file doesn't exist!\n", stdout);
		printf("\n");
		return;
	}

	if (fromOffset > toOffset)
	{
		printf("\n");
		printColor("red", "ERROR: Your starting index is greater than your ending index.\n", stdout);
		printf("\n");
		return;
	}
	
	//Read in the file header
	thisFileData = getFileData(filePtr, fileLoc);
	if (toOffset > thisFileData.fileSize)
	{
		printf("\n");
		printColor("red", "ERROR: Attempt to read beyond end of file.\n", stdout);
		printf("\n");
		return;
	}

	printf("\n");
	printColorTemplate("panther", "Printing file contents ...\n", stdout);
	printColorTemplate("panther", "-----------------------------------------\n", stdout);
	//Find the address of the data
	currentClusterNumber = getFirstClusterOfEntry(thisBootSector, thisFileData);

	//Convert the current cluster number 
	startingByteAddress = convertSectorNumToBytes(thisBootSector, getFirstSectorOfCluster(thisBootSector, currentClusterNumber));

	while (currentClusterNumber < EOC) {
		firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, currentClusterNumber);		
	
		for (sectorNumber = firstSectorOfCluster; sectorNumber < (firstSectorOfCluster + thisBootSector.sectorsPerCluster); sectorNumber++) {
			sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, sectorNumber);	
			
			//We're sorry
			for	(byteAddress = sectorNumberInBytes; byteAddress < (sectorNumberInBytes + thisBootSector.bytesPerSector); byteAddress += 1) {
				if (byteAddress >= (startingByteAddress + toOffset)) {
					printf("\n");
					printColorTemplate("panther", "-----------------------------------------\n", stdout);
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
	printColorTemplate("panther", "-----------------------------------------\n", stdout);
	printf("\n");
}

void printHelp() {
	printf("\n");
	printColorTemplate("panther", "Welcome to Panther Prompt's help manual\n", stdout);
	printColorTemplate("panther", "-----------------------------------------------------------------------------------------------------------------------\n", stdout);
	printColor("blue", "Here is a list of available commands...\n", stdout);
	printf(" > ls <directory_name>             -> list all files in <directory_name>; <directory_name> defaults to '.'i\n");
	printf(" > cd <directory_name>             -> change your directory to <directory_name>\n");
	printf(" > stat <file_name>                -> display file stats on <file_name>\n");
	printf(" > read <archive_name> <from> <to> -> read file <archive_name> from <from> to <to>; <from> and <to> must be integers\n");
	printf(" > quit                            -> quit Panther Prompt =( \n");
	printf(" > help                            -> display this help screen \n");
	printColor("blue", "Some issues we are aware of...\n", stdout);
	printf(" > ls .. is broken for root directory\n");
	printf(" > calling cd .. after cd . is broken\n");
	printf(" > we are assuming there is a volume name on the file system image\n");
	printf("(These fixes will be released in further releases?)\n");
	printColorTemplate("panther", "-----------------------------------------------------------------------------------------------------------------------\n", stdout);
	printf("\n");	
}

void replaceNewLine(char* string) {
    int index = 0;

    while (string[index] != '\n' && string[index] != '\0') {
        index++;
    }
    string[index] = '\0';
}


