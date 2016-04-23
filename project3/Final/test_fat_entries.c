///////////////////////////////////////////////////////////////////////////
// INCLUDES ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#include "fat_entries.h"

///////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
FILE* filePtr;
bootSector thisBootSector;

///////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// In:			thisBootSector (bootSector (struct)), clusterNumber (uint32_t)
// Out:			thisFatEntry (fatEntry (struct))
// Purpose:		to create a fatEntry struct for convenienve in moving through clusters and to abstract cluster chain logic	
// Notes:
void testGetFATEntry(int testNumber);

///////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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

	testGetFATEntry(1);
	testGetFATEntry(2);
	testGetFATEntry(3);
	testGetFATEntry(4);
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITONS ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void testGetFATEntry(int testNumber) {
	FATEntry thisFATEntry;
	

	printf("Testing getFATEntry()...\n");

	if (testNumber == 1) {
		printf("Running test 1...\n");
		thisFATEntry = getFATEntry(filePtr, thisBootSector, 4);  
		printf("Cluster Number: 4   | Expected = 5;   Actual = %d\n", thisFATEntry.nextClusterNumber);	
	}
	else if (testNumber == 2) {
		printf("Running test 2...\n");
		thisFATEntry = getFATEntry(filePtr, thisBootSector, 13);  
		printf("Cluster Number: 13  | Expected = 14;  Actual = %d\n", thisFATEntry.nextClusterNumber);	
	}
	else if (testNumber == 3 ) {
		printf("Running test 3...\n");
		thisFATEntry = getFATEntry(filePtr, thisBootSector, 514);  
		printf("Cluster Number: 514 | Expected = 515; Actual = %d\n", thisFATEntry.nextClusterNumber);	
	}
	else if (testNumber == 4) {	
		printf("Running test 4...\n");
		thisFATEntry = getFATEntry(filePtr, thisBootSector, 2);  
		printf("Cluster Number: 2 | Expected = %d;    Actual = %d\n", EOC, thisFATEntry.nextClusterNumber);	
	}
}
	
