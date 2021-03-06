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

uint64_t convertClusterNumToBytes(bootSector thisBootSector, uint32_t clusterNumber) {
	uint32_t firstSectorOfCluster;
	uint64_t clusterNumberInBytes;

	firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, clusterNumber);
	clusterNumberInBytes = convertSectorNumToBytes(thisBootSector, firstSectorOfCluster);
	return clusterNumberInBytes;
}

uint32_t convertBytesToClusterNum(bootSector thisBootSector, uint64_t byteAddress) {
	uint32_t firstDataSector;
	firstDataSector = getFirstDataSector(thisBootSector);
	return ((((byteAddress / thisBootSector.bytesPerSector) - firstDataSector) / thisBootSector.sectorsPerCluster) + 2); 
}
