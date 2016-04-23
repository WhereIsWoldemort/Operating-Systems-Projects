//////////////////////////////////////////////////////////////////////////////////
// INCLUDES ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "boot_sector.h"
#include <sys/types.h>

//////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bootSector setUpBootSector(FILE* filePtr) {
	bootSector thisBootSector;		// stores the boot sector returned

	// bytes per sector
	fseek(filePtr, BPS_OFFSET, SEEK_SET); 
	fread(&(thisBootSector.bytesPerSector), READ_RESOLUTION, BPS_SIZE, filePtr);
	thisBootSector.bytesPerSector = le16toh(thisBootSector.bytesPerSector);	

	// sectors per cluster (don't need to switch because SPC_SIZE = 1)
	fseek(filePtr, SPC_OFFSET, SEEK_SET);
	fread(&(thisBootSector.sectorsPerCluster), READ_RESOLUTION, SPC_SIZE, filePtr);

	// reserved sector count
	fseek(filePtr, RSC_OFFSET, SEEK_SET);
	fread(&(thisBootSector.reservedSectorCount), READ_RESOLUTION, RSC_SIZE, filePtr);
	thisBootSector.reservedSectorCount = le16toh(thisBootSector.reservedSectorCount);

	// number of FATs (don't need to switch because NF_SIZE = 1)
	fseek(filePtr, NF_OFFSET, SEEK_SET);
	fread(&(thisBootSector.numOfFATs), READ_RESOLUTION, NF_SIZE, filePtr);	

	// size of FAT
	fseek(filePtr, FS_OFFSET, SEEK_SET);
	fread(&(thisBootSector.fatSize), READ_RESOLUTION, FS_SIZE, filePtr);		
	thisBootSector.fatSize = le32toh(thisBootSector.fatSize);	

	// root cluster number
	fseek(filePtr, RC_OFFSET, SEEK_SET);
	fread(&(thisBootSector.rootClusterNum), READ_RESOLUTION, RC_SIZE, filePtr);
	thisBootSector.rootClusterNum = le32toh(thisBootSector.rootClusterNum);

	// total number of sectors
	fseek(filePtr, TS_OFFSET, SEEK_SET);
	fread(&(thisBootSector.totalSectors), READ_RESOLUTION, TS_SIZE, filePtr);
	thisBootSector.totalSectors = le32toh(thisBootSector.totalSectors);

	// root entry count
	fseek(filePtr, REC_OFFSET, SEEK_SET);
	fread(&(thisBootSector.rootEntryCount), READ_RESOLUTION, REC_SIZE, filePtr);
	thisBootSector.rootEntryCount = le16toh(thisBootSector.rootEntryCount);

	//TODO do this in a less redundant way
	thisBootSector.rootDirectorySectors = ((thisBootSector.rootEntryCount * 32) + (thisBootSector.bytesPerSector - 1)) / thisBootSector.bytesPerSector; 
	thisBootSector.firstDataSector  = thisBootSector.reservedSectorCount + (thisBootSector.numOfFATs * thisBootSector.fatSize) + thisBootSector.rootDirectorySectors;	
	
	return thisBootSector;
	printf("bytesPerSector: %d\n", thisBootSector.bytesPerSector);

}
