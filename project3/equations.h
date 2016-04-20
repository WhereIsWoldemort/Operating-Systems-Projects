#include <stdint.h>
#include "boot_sector.h"

///////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS //////////////////////////////////////
///////////////////////////////////////////////////////////////

// In:			rootEntryCount (uint16_t), bytesPerSector (uint16_t)	
// Out:			numOfSectors (uint32_t)
// Purpose: 	to get the number of sectors in the root directory 
// Notes:		
uint32_t getRootDirectorySectors(bootSector thisBootSector);

// In:			reservedSectorCount (uint16_t), numOfFATs (uint8_t), fatSize (uint32_t), rootDirectorySectors (uint32_t)
// Out:			sectorNumber (uint32_t)
// Purpose:		to get the first sector number of the data region
// Notes:
uint32_t getFirstDataSector(bootSector thisBootSector); 

// In:			numOfCluster (uint32_t), sectorsPerCluster (uint32_t), firstDataSector (uint32_t)
// Out:			sectorNumber (uint32_t)	
// Purpose: 	to get the first sector number of a cluster
// Notes:		this is automatically adjusted based on the first data sector number
uint32_t getFirstSectorOfCluster(bootSector thisBootSector, uint32_t numOfCluster);

// In:			numOfCluster (uint32_t), reservedSectorCount (uint16_t), bytesPerSector (uint16_t)
// Out:			sectorNumber (uint32_t)
// Purpose:		to get the sector number of the FAT entry for this cluster
// Notes:		this applies to the FAT REGION not the data region
uint32_t getThisFATSectorNumber(bootSector thisBootSector, uint32_t numOfCluster);

// In:			numOfCluster (uint32_t), bytesPerSector (uint32_t)
// Out:			byteOffset (uint16_t)
// Purpose:		to get the byte address offset within the sector number when determining the FAT entry location
// Notes:		
uint16_t getThisFATEntryOffset(bootSector thisBootSector, uint32_t numOfCluster);

// In:			sectorNumber (uint32_t), bytesPerSector (uint32_t)
// Out:			byteAddress (uint64_t)
// Purpose:		to return the byte address of a given sector number
// Notes:
uint64_t convertSectorNumToBytes(bootSector thisBootSector, uint32_t sectornumber);



