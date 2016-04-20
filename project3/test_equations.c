#include <stdio.h>
#include <stdint.h>
#include "equations.h"
//#include "boot_sector.h" INHERITED FROM EQUATIONS

////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES ////////////////////////////////////////////
////////////////////////////////////////////////////////////////
FILE* filePtr;
bootSector thisBootSector;

////////////////////////////////////////////////////////////////
// FUNCTION DELCARATIONS ///////////////////////////////////////
////////////////////////////////////////////////////////////////
void testGetRootDirectorySectors(int testNumber);
void testGetFirstDataSector(int testNumber);
void testGetFirstSectorOfCluster(int testNumber);
void testGetThisFATSectorNumber(int testNumber);
void testGetThisFATEntryOffset(int testNumber);
void testConvertSectorNumToBytes(int testNumber); 
void testConvertClusterNumToBytes(int testNumber);

////////////////////////////////////////////////////////////////
// MAIN PROGRAM ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

	// set up file pointer	
	if (argc != 2) {
		printf("You entered an incorrect amount of arguments for this program. <USAGE: ./fat32_reader fat32.img>\n");
		return -1;
	}

	if ((filePtr = fopen(argv[1], "rb")) == 0) {
		perror(argv[1]);
		return -1;
	}

	// set up the boot sector 
	thisBootSector = setUpBootSector(filePtr); 

	// run the actual tests!
	testGetRootDirectorySectors(1);
	testGetFirstDataSector(1);
	testGetFirstSectorOfCluster(1);
	testGetThisFATSectorNumber(1);
	testGetThisFATEntryOffset(1);
	testConvertSectorNumToBytes(1); 
	testConvertClusterNumToBytes(1);

	return 0;
}

////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS ////////////////////////////////////////
////////////////////////////////////////////////////////////////

// tests: 	getRootDirectorySectors()
// from: 	equations.c
void testGetRootDirectorySectors(int testNumber) {
	uint32_t rootDirectorySectors;
	printf("Testing getRootDirectorySectors()...\n");	
	rootDirectorySectors = getRootDirectorySectors(thisBootSector);
	printf("Expected Value = 0;     Actual Value = %d\n", rootDirectorySectors);
}

// tests: 	getGetFirstDataSector()
// from: 	equations.void testGetFirstDataSector(int testNumber) {
void testGetFirstDataSector(int testNumber) {
	uint32_t firstDataSectorNumber;
	printf("Testing getFirstDataSector()...\n");	
	firstDataSectorNumber = getFirstDataSector(thisBootSector);
	printf("Expected Value = 2050;  Actual Value = %d\n", firstDataSectorNumber);

}

// tests: 	getFirstSectorOfCluster()
// from: 	equations.c
void testGetFirstSectorOfCluster(int testNumber) {
	uint32_t firstSectorOfCluster;
	printf("Testing getFirstSectorOfCluster()...\n");	
	firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, 4);
	printf("Expected Value = 2052;  Actual Value = %d\n", firstSectorOfCluster);
}

// tests: 	getThisFATSectorNumber()
// from: 	equations.c
void testGetThisFATSectorNumber(int testNumber) {
	uint32_t thisFATSectorNumber;
	printf("Testing getThisFATSectorNumber()...\n");	
	thisFATSectorNumber = getThisFATSectorNumber(thisBootSector, 150);
	printf("Expected Value = 33;    Actual Value = %d\n", thisFATSectorNumber);

}

// tests: 	getThisFATEntryOffset()
// from: 	equations.c
void testGetThisFATEntryOffset(int testNumber) { 
	uint16_t thisFATEntryOffset;
	printf("Testing getThisFATEntryOffset()...\n");	
	thisFATEntryOffset = getThisFATEntryOffset(thisBootSector, 150);
	printf("Expected Value = 88;    Actual Value = %d\n", thisFATEntryOffset);

}

// tests: 	ConvertSectorNumToBytes()
// from: 	equations.c
void testConvertSectorNumToBytes(int testNumber) {
	uint64_t sectorNumberInBytes;
	printf("Testing convertSectorNumToBytes()...\n");	
	sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, 45);
	printf("Expected Value = 23040; Actual Value = %d\n", sectorNumberInBytes);

}

void testConvertClusterNumToBytes(int testNumber) {
	uint64_t clusterNumberInBytes;
	printf("Testing convertClusterNumToBytes()...\n");
	clusterNumberInBytes = convertClusterNumToBytes(thisBootSector, 403);
	printf("Expected Value = 1254912;  Actual Value = %d\n", clusterNumberInBytes);
} 
