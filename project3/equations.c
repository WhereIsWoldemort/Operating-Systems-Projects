#include "equations.h"

uint32_t getRootDirectorySectors(bootSector thisBootSector) {
	return (((thisBootSector.rootEntryCount * 32) + (thisBootSector.bytesPerSector - 1)) / thisBootSector.bytesPerSector); 
}

uint32_t getFirstDataSector(bootSector thisBootSector) {
	return (thisBootSector.reservedSectorCount + (thisBootSector.numOfFATs * thisBootSector.fatSize) + thisBootSector.rootDirectorySectors);	
} 

uint32_t getFirstSectorOfCluster(bootSector thisBootSector, uint32_t numOfCluster) {
	return (((numOfCluster - 2) * thisBootSector.sectorsPerCluster) + thisBootSector.firstDataSector);
}

uint32_t getThisFATSectorNumber(bootSector thisBootSector, uint32_t numOfCluster) {
	uint64_t fatOffset;
	fatOffset = numOfCluster * 4;
	return (thisBootSector.reservedSectorCount + (fatOffset / thisBootSector.bytesPerSector)); 
}

uint16_t getThisFATEntryOffset(bootSector thisBootSector, uint32_t numOfCluster) {
	uint64_t fatOffset;
	fatOffset = numOfCluster * 4;
	return (fatOffset % thisBootSector.bytesPerSector);
}

uint64_t convertSectorNumToBytes(bootSector thisBootSector, uint32_t sectorNumber) {
	return sectorNumber * thisBootSector.bytesPerSector;
}


