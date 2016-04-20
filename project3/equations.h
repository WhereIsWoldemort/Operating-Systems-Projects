#include <stdint.h>
#include "boot_sector.h"

///////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS //////////////////////////////////////
///////////////////////////////////////////////////////////////

// In:			thisBootSector (bootSector (struct))	
// Out:			numOfSectors (uint32_t)
// Purpose: 	to get the number of sectors in the root directory 
// Notes:		
uint32_t getRootDirectorySectors(bootSector thisBootSector);

// In:			thisBootSector (bootSector (struct))
// Out:			sectorNumber (uint32_t)
// Purpose:		to get the first sector number of the data region
// Notes:
uint32_t getFirstDataSector(bootSector thisBootSector); 

// In:			thisBootSector (bootSector (struct))
// Out:			sectorNumber (uint32_t)	
// Purpose: 	to get the first sector number of a cluster
// Notes:		this is automatically adjusted based on the first data sector number
uint32_t getFirstSectorOfCluster(bootSector thisBootSector, uint32_t numOfCluster);

// In:			thisBootSector (bootSector (struct)), numOfCluster (uint32_t)
// Out:			sectorNumber (uint32_t)
// Purpose:		to get the sector number of the FAT entry for this cluster
// Notes:		this applies to the FAT REGION not the data region
uint32_t getThisFATSectorNumber(bootSector thisBootSector, uint32_t numOfCluster);

// In: 			thisBootSector (bootSector (struct)), numOfCluster (uint32_t)
// Out:			fatByteOffset (uint16_t)
// Purpose:		to get the byte address offset within the sector number when determining the FAT entry location
// Notes:		
uint16_t getThisFATEntryOffset(bootSector thisBootSector, uint32_t numOfCluster);

// In: 			thisBootSector (bootSector (struct)), sectorNumber (uint32_t)
// Out:			sectorNumInBytes (uint64_t)
// Purpose:		to return the byte address of a given sector number
// Notes:
uint64_t convertSectorNumToBytes(bootSector thisBootSector, uint32_t sectorNumber);

// In: 			thisBootSector (bootSector (struct)), clusterNumber (uint32_t)
// Out:			clusterNumInBytes (uint64_t)
// Purpose:		to return the byte address of a given cluster number
// Notes:
uint64_t convertClusterNumToBytes(bootSector thisBootSector, uint32_t clusterNumber);

