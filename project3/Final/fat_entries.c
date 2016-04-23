///////////////////////////////////////////////////////////////////////////
// INCLUDES ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#include "fat_entries.h"
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
FATEntry getFATEntry(FILE* filePtr, bootSector thisBootSector, uint32_t clusterNumber) {
	uint32_t thisFATEntrySectorNumber; 		// the sector number of thisFATEntry
	uint16_t thisFATEntryByteOffset; 		// the byte offset within the sector of thisFATentry
	uint64_t thisFATEntryByteAddress;		// the byte address of the fat entry
	uint32_t nextClusterNumber;				// the next cluster number in the cluster chain
	FATEntry thisFATEntry;					// the FAT entry structure we create and return		
	
	// 1. determine the sector number index of the FAT for clusterNumber
	thisFATEntrySectorNumber = getThisFATSectorNumber(thisBootSector, clusterNumber);		 

	// 2. determine the byte offset into the determined sector number
	thisFATEntryByteOffset = getThisFATEntryOffset(thisBootSector, clusterNumber);

	// 3. convert sector number and offset into byte address
	thisFATEntryByteAddress = convertSectorNumToBytes(thisBootSector, thisFATEntrySectorNumber) + thisFATEntryByteOffset;

	// 3. read the 4-byte entry
	fseek(filePtr, thisFATEntryByteAddress, SEEK_SET);
	fread(&nextClusterNumber, READ_RESOLUTION, FAT_ENTRY_SIZE, filePtr);
	nextClusterNumber = le32toh(nextClusterNumber);	

	// 4. build and return the FAT entry
	thisFATEntry.currentClusterNumber 	= clusterNumber;
	thisFATEntry.nextClusterNumber 		= nextClusterNumber;
	thisFATEntry.FATEntrySectorNumber	= thisFATEntrySectorNumber;
	thisFATEntry.FATEntryByteOffset		= thisFATEntryByteOffset;
	thisFATEntry.FATEntryByteAddress	= thisFATEntryByteAddress;

	return thisFATEntry;

} 

