////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#include <stdio.h>
#include "directory_entries.h"
#include <string.h>

/////////////////////////////////////////////////////////////
// MACROS ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#define ENTRIES_COUNT 5

//////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////
//////////////////////////////////////////////////////////////
void testEndOfDirectory(int testNumber);
void setUpSystem();
void testSetUp();
void testIsEmptyDirectoryEntry(int testNumber);
void testHasLongName(int testNumber);
void testIsDirectory(int testNumber);
void testIsFile(int testNumber);
void testIsVolumeLabel(int testNumber);
void testIsReadOnly(int testNumber);
void testGetFirstClusterOfEntry(int testNumber);

//////////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////////
//////////////////////////////////////////////////////////////
fileData directoryEntry_1;
fileData directoryEntry_2;
fileData directoryEntry_3;
fileData directoryEntry_4;
fileData directoryEntry_5;

fileData system[ENTRIES_COUNT];

//////////////////////////////////////////////////////////////
// MAIN PROGRAM //////////////////////////////////////////////
//////////////////////////////////////////////////////////////
int main() {
	setUpSystem();	
	testIsReadOnly(1);
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
	directoryEntry_2.firstClusterNumHI 	= 0;
	directoryEntry_2.firstClusterNumLO 	= 0;
	directoryEntry_2.writeTime 			= 0;
	directoryEntry_2.writeDate 			= 0;
	directoryEntry_2.fileSize 			= 0;
	
	// initialize directoryEntry_3
	char fileName3[SH_FILE_NAME + 1] = "GHOST.TXT";	
	strcpy(directoryEntry_3.fileName, fileName3);
	directoryEntry_3.fileName[0] 		= 5;
	directoryEntry_3.fileAttributes 	= 32; // 00100000	
	directoryEntry_3.createTime 		= 0;
	directoryEntry_3.createDate 		= 0;
	directoryEntry_3.lastAccessDate 	= 0;
	directoryEntry_3.firstClusterNumHI 	= 0;
	directoryEntry_3.firstClusterNumLO 	= 0;
	directoryEntry_3.writeTime 			= 0;
	directoryEntry_3.writeDate 			= 0;
	directoryEntry_3.fileSize 			= 0;
	
	// initialize directoryEntry_4
	char fileName4[SH_FILE_NAME + 1] = "BANANA.TXT";
	strcpy(directoryEntry_4.fileName, fileName4);
	directoryEntry_4.fileAttributes 	= 33; // 00100001	
	directoryEntry_4.createTime 		= 0;
	directoryEntry_4.createDate 		= 0;
	directoryEntry_4.lastAccessDate 	= 0;
	directoryEntry_4.firstClusterNumHI 	= 0;
	directoryEntry_4.firstClusterNumLO 	= 0;
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
	
	system[0] = directoryEntry_1;
	system[1] = directoryEntry_2;
	system[2] = directoryEntry_3;
	system[3] = directoryEntry_4;
	system[4] = directoryEntry_5;
}

// tests: 	setUpSystem()
// from: 	here
void testSetUp() {
	int i;
	fileData thisFileData;

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
	int expectedValues[ENTRIES_COUNT] = {0, 1, 0, 0, 0};
	int actualValue;
	int i;

	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isDirectory(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
	}
}

void testIsFile(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 1, 1, 0};
	int actualValue;
	int i;

	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isFile(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsEmptyDirectoryEntry(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 1, 0, 0};
	int actualValue;
	int i;

	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isEmptyDirectoryEntry(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsReadOnly(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {0, 0, 0, 1, 0};
	int actualValue;
	int i;

	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isReadOnly(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}

void testIsVolumeLabel(int testNumber) {
	int expectedValues[ENTRIES_COUNT] = {1, 0, 0, 0, 0};
	int actualValue;
	int i;

	for (i = 0; i < ENTRIES_COUNT; i++) {
		actualValue = isVolumeLabel(system[i]);
		printf("Expected Value = %d; Actual Value = %d\n", expectedValues[i], actualValue); 
		
	} 
}
	

