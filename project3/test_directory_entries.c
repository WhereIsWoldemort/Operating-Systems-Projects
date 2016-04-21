////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#include <stdio.h>
#include "directory_entries.h"
#include <string.h>
#include "equations.h"
/////////////////////////////////////////////////////////////
// MACROS ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#define ENTRIES_COUNT 6

//////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////
//////////////////////////////////////////////////////////////
void testEndOfDirectory(int testNumber);
void setUpSystem();
void testSetUp();
void testIsEmptyDirectoryEntry(int testNumber);
void testIsLongName(int testNumber);
void testIsDirectory(int testNumber);
void testIsFile(int testNumber);
void testIsVolumeLabel(int testNumber);
void testIsReadOnly(int testNumber);
void testCheckFileExists(int testNumber);
void testGetFirstClusterOfEntry(int testNumber);
void testConvertToDateStruct(int testNumber);
void testConvertToTimeStruct(int testNumber);

//////////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////////
//////////////////////////////////////////////////////////////
fileData directoryEntry_1;
fileData directoryEntry_2;
fileData directoryEntry_3;
fileData directoryEntry_4;
fileData directoryEntry_5;
fileData directoryEntry_6;

fileData system[ENTRIES_COUNT];

FILE* filePtr;
bootSector thisBootSector;

//////////////////////////////////////////////////////////////
// MAIN PROGRAM //////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

	if (argc != 2) {
		printf("You entered an incorrect amount of arguments for this program. <USAGE: ./fat32_reader fat32.img>\n");
		return -1;
	}

	if ((filePtr = fopen(argv[1], "rb")) == 0) {
		perror(argv[1]);
		return -1;
	}

	thisBootSector = setUpBootSector(filePtr);

	setUpSystem();
	testIsReadOnly(2);
	testIsFile(3);
	testIsDirectory(5);
	testIsEmptyDirectoryEntry(5);	
	testIsVolumeLabel(1);
	testIsLongName(2);
	testConvertToDateStruct(1);
	testConvertToDateStruct(2);
	testConvertToTimeStruct(1);
	testConvertToTimeStruct(2);
	testCheckFileExists(1);
	testCheckFileExists(2);
	testCheckFileExists(3);	
	testCheckFileExists(4);	
	testCheckFileExists(5);	
	testGetFirstClusterOfEntry(1);


	fclose(filePtr);
}

//////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS //////////////////////////////////////
//////////////////////////////////////////////////////////////

void setUpSystem() {
	// initialize directoryEntry_1
	char fileName1[SH_FILE_NAME + 1] = "CHUCKLES";
	strcpy(directoryEntry_1.fileName, fileName1);
	directoryEntry_1.fileAttributes 	= 8; // 00001000 	
	directoryEntry_1.createTime 		= 0;
	directoryEntry_1.createDate 		= 0;
	directoryEntry_1.lastAccessDate 	= 0;
	directoryEntry_1.firstClusterNumHI 	= 0;
	directoryEntry_1.firstClusterNumLO 	= 0;
	directoryEntry_1.writeTime 			= 0;
	directoryEntry_1.writeDate 			= 0;
	directoryEntry_1.fileSize 			= 0;

	// initialize directoryEntry_2
	char fileName2[SH_FILE_NAME + 1] = "POOP";	
	strcpy(directoryEntry_2.fileName, fileName2);
	directoryEntry_2.fileAttributes 	= 16; // 00010000	
	directoryEntry_2.createTime 		= 0;
	directoryEntry_2.createDate 		= 0;
	directoryEntry_2.lastAccessDate 	= 0;
	directoryEntry_2.firstClusterNumHI 	= 2;
	directoryEntry_2.firstClusterNumLO 	= 43;
	directoryEntry_2.writeTime 			= 0;
	directoryEntry_2.writeDate 			= 0;
	directoryEntry_2.fileSize 			= 0;
	
	// initialize directoryEntry_3
	char fileName3[SH_FILE_NAME + 1] = "GHOST.TXT";	
	strcpy(directoryEntry_3.fileName, fileName3);
	directoryEntry_3.fileName[0] 		= -27;
	directoryEntry_3.fileAttributes 	= 32; 		// 0010 0000	
	directoryEntry_3.createTime 		= 43195; 	// 1010 1000 1011 1011
	directoryEntry_3.createDate 		= 18613;	// 0100 1000 1011 0101
	directoryEntry_3.lastAccessDate 	= 0;
	directoryEntry_3.firstClusterNumHI 	= 0;
	directoryEntry_3.firstClusterNumLO 	= 0;
	directoryEntry_3.writeTime 			= 0;
	directoryEntry_3.writeDate 			= 0;
	directoryEntry_3.fileSize 			= 0;
	
	// initialize directoryEntry_4
	char fileName4[SH_FILE_NAME + 1] = "BANANA.TXT";
	strcpy(directoryEntry_4.fileName, fileName4);
	directoryEntry_4.fileAttributes 	= 33; 		// 0010 0001	
	directoryEntry_4.createTime 		= 12157;	// 0010 1111 0111 1101
	directoryEntry_4.createDate 		= 10830;	// 0010 1010 0100 1110
	directoryEntry_4.lastAccessDate 	= 0;
	directoryEntry_4.firstClusterNumHI 	= 0;
	directoryEntry_4.firstClusterNumLO 	= 54;
	directoryEntry_4.writeTime 			= 0;
	directoryEntry_4.writeDate 			= 0;
	directoryEntry_4.fileSize 			= 342;

	// initialize directoryEntry_5
	char fileName5[SH_FILE_NAME + 1] = "\0";	
	strcpy(directoryEntry_5.fileName, fileName5);
	directoryEntry_5.fileAttributes 	= 0;
	directoryEntry_5.createTime 		= 0;
	directoryEntry_5.createDate 		= 0;
	directoryEntry_5.lastAccessDate 	= 0;
	directoryEntry_5.firstClusterNumHI 	= 0;
	directoryEntry_5.firstClusterNumLO 	= 0;
	directoryEntry_5.writeTime 			= 0;
	directoryEntry_5.writeDate 			= 0;
	directoryEntry_5.fileSize 			= 0;

	// initialize directoryEntry_6	
	char fileName6[SH_FILE_NAME + 5] = "POOPOO";	
	strcpy(directoryEntry_6.fileName, fileName6);
	directoryEntry_6.fileAttributes 	= 15;
	directoryEntry_6.createTime 		= 0;
	directoryEntry_6.createDate 		= 0;
	directoryEntry_6.lastAccessDate 	= 0;
	directoryEntry_6.firstClusterNumHI 	= 0;
	directoryEntry_6.firstClusterNumLO 	= 0;
	directoryEntry_6.writeTime 			= 0;
	directoryEntry_6.writeDate 			= 0;
	directoryEntry_6.fileSize 			= 0;

	system[0] = directoryEntry_1;
	system[1] = directoryEntry_2;
	system[2] = directoryEntry_3;
	system[3] = directoryEntry_4;
	system[4] = directoryEntry_5;
	system[5] = directoryEntry_6;
}

// tests: 	setUpSystem()
// from: 	here
void testSetUp() {
	int i;
	fileData thisFileData;

	printf("Testing set up...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		thisFileData = system[i];
		printf("%s\n", thisFileData.fileName);		
		printf("%d\n", thisFileData.fileAttributes);		
		printf("%d\n", thisFileData.createTime);		
		printf("%d\n", thisFileData.createDate);		
		printf("%d\n", thisFileData.lastAccessDate);		
		printf("%d\n", thisFileData.firstClusterNumHI);		
		printf("%d\n", thisFileData.firstClusterNumLO);		
		printf("%d\n", thisFileData.writeTime);		
		printf("%d\n", thisFileData.writeDate);		
		printf("%d\n", thisFileData.fileSize);
		printf("\n\n");		
	}	
}

// tests: 	isEndOfDirectory() 
// from: 	booleans.h
void testEndOfDirectory(int testNumber) {
	int i;		// looping variable
	
	printf("Testing end of directory...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		if (isEndOfDirectory(system[i])) {
			printf("HOLY CRAP! IT IS THE END!\n");
		}
		else {
			printf("Nope... it's not over yet.\n");
		}
	}
}

void testIsDirectory(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 1, 0, 0, 0, 0};
	int actualValue;
	int i;

	printf("Testing is directory...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isDirectory(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
	}
}

void testIsFile(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 1, 1, 0, 0};
	int actualValue;
	int i;
	
	printf("Testing is file...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isFile(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsEmptyDirectoryEntry(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 1, 0, 0, 0};
	int actualValue;
	int i;

	printf("Testing is empty directory entry...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isEmptyDirectoryEntry(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsReadOnly(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 0, 1, 0, 0};
	int actualValue;
	int i;

	
	printf("Testing is read only...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isReadOnly(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsVolumeLabel(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {1, 0, 0, 0, 0, 0};
	int actualValue;
	int i;

	printf("Testing is volume label...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isVolumeLabel(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}
	
void testIsLongName(int testNumber) {	
	int expectedValues[ENTRIES_COUNT] = {0, 0, 0, 0, 0, 1};
	int actualValue;
	int i;

	printf("Testing is long name...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isLongName(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 		
	} 
}

void testCheckFileExists(int testNumber) {
	int directoryEntryAddress;
	uint64_t rootByteAddress;
	
	printf("Testing checkFileExists()...\n");
	
	rootByteAddress = convertClusterNumToBytes(thisBootSector, thisBootSector.rootClusterNum);

	if (testNumber == 1) {
		printf("Running test 1...\n");
		directoryEntryAddress = checkFileExists(filePtr, "FSINFO  TXT", rootByteAddress);
		printf("Expected Value = 1049664; Actual Value = %d\n", directoryEntryAddress);
	}
	else if (testNumber == 2) {
		printf("Running test 2...\n");
		directoryEntryAddress = checkFileExists(filePtr, "DIR", rootByteAddress);
		printf("Expected Value = 1049920; Actual Value = %d\n", directoryEntryAddress);
	}
	else if (testNumber == 3) {
		printf("Running test 3...\n");
		directoryEntryAddress = checkFileExists(filePtr, ".ECRET  TXT", rootByteAddress);
		printf("Expected Value = -1; Actual Value = %d\n", directoryEntryAddress);
	}
	else if (testNumber == 4) {
		printf("Running test 4...\n");
		directoryEntryAddress = checkFileExists(filePtr, "CHUCKLES", rootByteAddress);
		printf("Expected Value = -1; Actual Value = %d\n", directoryEntryAddress);
	}
	else if (testNumber == 5) {
		printf("Running test 5...\n");
		directoryEntryAddress = checkFileExists(filePtr, "POOPOOBA", rootByteAddress);
		printf("Expected Value = -1; Actual Value = %d\n", directoryEntryAddress);
	}
}

void testGetFirstClusterOfEntry(int testNumber) {
	int i;	
	int expectedValues[ENTRIES_COUNT] = {0, 131115, 0, 54, 0, 0};
	uint32_t actualValue;
	
	printf("Testing getFirstClusterOfEntry()...\n");
	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = getFirstClusterOfEntry(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 		
	}
}

void testConvertToDateStruct(int testNumber) {	
	date thisDate;	

	printf("Testing convertToDateStruct()...\n");
	if (testNumber == 1) {
		printf("Running test 1...\n");
		thisDate = convertToDateStruct(directoryEntry_3.createDate);
		printf("Expected = 05-21-2016; Actual = %02d-%02d-%02d\n", thisDate.month, thisDate.day, thisDate.year);
	}
	else if (testNumber == 2) {
		printf("Running test 2...\n");
		thisDate = convertToDateStruct(directoryEntry_4.createDate);
		printf("Expected = 02-14-2001; Actual = %02d-%02d-%02d\n", thisDate.month, thisDate.day, thisDate.year);
	}
}

void testConvertToTimeStruct(int testNumber) {
	time thisTime;

	printf("Testing convertToTimeStruct()... \n");
	if (testNumber == 1) {
		printf("Running test 1...\n");
		thisTime = convertToTimeStruct(directoryEntry_3.createTime);
		printf("Expected = 21:05:54; Actual = %02d:%02d:%02d\n", thisTime.hours, thisTime.minutes, thisTime.seconds);
	}
	else if (testNumber == 2) {
		printf("Running test 2...\n");
		thisTime = convertToTimeStruct(directoryEntry_4.createTime);
		printf("Expected = 05:59:58; Actual = %02d:%2d:%02d\n", thisTime.hours, thisTime.minutes, thisTime.seconds);
	}
}
