//////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#include "equations.h"

//////////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////////
//////////////////////////////////////////////////////////////
bootSector thisBootSector;
FILE* filePtr;

//////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////
//////////////////////////////////////////////////////////////
void testSetUpBootSector(int testNumber);

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

	testSetUpBootSector(4);
}

void testSetUpBootSector(int testNumber) { 
	thisBootSector = setUpBootSector(filePtr);

	printf("bytesPerSector:       Expected = 512,    Actual = %d\n", thisBootSector.bytesPerSector); 
	printf("sectorsPerCluster:    Expected = 1,      Actual = %d\n", thisBootSector.sectorsPerCluster); 
	printf("reservedSectorCount:  Expected = 32,     Actual = %d\n", thisBootSector.reservedSectorCount); 
	printf("numOfFATs:            Expected = 2,      Actual = %d\n", thisBootSector.numOfFATs); 
	printf("fatSize:              Expected = 1009,   Actual = %d\n", thisBootSector.fatSize); 
	printf("rootClusterNum:       Expected = 2,      Actual = %d\n", thisBootSector.rootClusterNum); 
	printf("rootEntryCount:       Expected = 0,      Actual = %d\n", thisBootSector.rootEntryCount); 
	printf("totalSectors:         Expected = 131072, Actual = %d\n", thisBootSector.totalSectors); 
	printf("rootDirectorySectors: Expected = 0,      Actual = %d\n", thisBootSector.rootDirectorySectors); 
	printf("firstDataSector:      Expected = 2050,   Actual = %d\n", thisBootSector.firstDataSector); 
}
