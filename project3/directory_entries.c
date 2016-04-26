#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "directory_entries.h"
#include "fat_entries.h"

fileData getFileData(FILE* filePtr, uint64_t byteAddress) {
	fileData thisFileData;
	
	// 1. jump to where the file data is
	fseek(filePtr, byteAddress, SEEK_SET);

	// 2. get the directory name
	fread(&(thisFileData.fileName), READ_RESOLUTION, DN_SIZE, filePtr);
	thisFileData.fileName[SH_DIRNAME_SIZE] = '\0';
	
	// 3. get the file attributes
	fread(&(thisFileData.fileAttributes), READ_RESOLUTION, DA_SIZE, filePtr);

	// 4. get the create time
	fseek(filePtr, DCT_OFFSET - (DA_OFFSET + DA_SIZE), SEEK_CUR);
	fread(&(thisFileData.createTime), READ_RESOLUTION, DCT_SIZE, filePtr);
	thisFileData.createTime = le16toh(thisFileData.createTime);

	// 5. get the create date
	fread(&(thisFileData.createDate), READ_RESOLUTION, DCD_SIZE, filePtr);
	thisFileData.createDate = le16toh(thisFileData.createDate);
	
	// 6. get the last access date
	fread(&(thisFileData.lastAccessDate), READ_RESOLUTION, DLAD_SIZE, filePtr);
	thisFileData.lastAccessDate = le16toh(thisFileData.lastAccessDate);

	// 7. get the first cluster number high order word
	fread(&(thisFileData.firstClusterNumHI), READ_RESOLUTION, DFCH_SIZE, filePtr);
	thisFileData.firstClusterNumHI = le16toh(thisFileData.firstClusterNumHI);

	// 8. get the last write time
	fread(&(thisFileData.writeTime), READ_RESOLUTION, DWT_SIZE, filePtr);
	thisFileData.writeTime = le16toh(thisFileData.writeTime);

	// 9. get the last write date
	fread(&(thisFileData.writeDate), READ_RESOLUTION, DWD_SIZE, filePtr);
	thisFileData.writeDate = le16toh(thisFileData.writeDate);

	// 10. get the first cluster number low order word
	fread(&(thisFileData.firstClusterNumLO), READ_RESOLUTION, DFCL_SIZE, filePtr);
	thisFileData.firstClusterNumLO = le16toh(thisFileData.firstClusterNumLO);

	// 11. get the file size
	fread(&(thisFileData.fileSize), READ_RESOLUTION, DFS_SIZE, filePtr);
	thisFileData.fileSize = le32toh(thisFileData.fileSize);

	return thisFileData;
}

BOOL isReadOnly(fileData thisFileData) {
	if (((thisFileData.fileAttributes & 1) == 1) && (thisFileData.fileAttributes & 15) != 15) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isSystemFile(fileData thisFileData) {
	if (((thisFileData.fileAttributes & 2) == 2) && (thisFileData.fileAttributes & 15) != 15) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isHiddenFile(fileData thisFileData) {
	if (((thisFileData.fileAttributes & 4) == 4) && (thisFileData.fileAttributes & 15) != 15) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isVolumeLabel(fileData thisFileData) {
	if (((thisFileData.fileAttributes & 8) == 8) && (thisFileData.fileAttributes & 15) != 15) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


BOOL isLongName(fileData thisFileData) {
	if ((thisFileData.fileAttributes & 15) == 15) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isDirectory(fileData thisFileData) {
	if ((thisFileData.fileAttributes & 16) == 16) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isFile(fileData thisFileData) {
	if ((thisFileData.fileAttributes & 32) == 32) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isEndOfDirectory(fileData thisFileData) {
	if (thisFileData.fileName[0] == '\0') {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isEmptyDirectoryEntry(fileData thisFileData) {	
	if (thisFileData.fileName[0] == -27) {	
		return TRUE;
	}
	else {
		return FALSE;
	}
}

uint64_t checkFileExists(FILE* filePtr, bootSector thisBootSector, char* fileName, uint32_t currentClusterNumber) {
	fileData thisFileData;					// 	
	FATEntry thisFATEntry;
	uint32_t firstSectorOfCluster;			// the first sector of the current cluster we're on
	uint32_t sectorNumber;					
	uint64_t sectorNumberInBytes;
	uint64_t currentByteAddress;		
	char* name; 
	char* ext;
	char toCompare[SH_DIRNAME_SIZE + 1];
	int i;									// looping variable

	while (currentClusterNumber != EOC) {
		firstSectorOfCluster = getFirstSectorOfCluster(thisBootSector, currentClusterNumber);		
	
		for (sectorNumber = firstSectorOfCluster; sectorNumber < (firstSectorOfCluster + thisBootSector.sectorsPerCluster); sectorNumber++) {
			sectorNumberInBytes = convertSectorNumToBytes(thisBootSector, sectorNumber);	
			
			for	(currentByteAddress = sectorNumberInBytes; currentByteAddress < (sectorNumberInBytes + thisBootSector.bytesPerSector); currentByteAddress += DIR_ENTRY_SIZE) {
				thisFileData = getFileData(filePtr, currentByteAddress);	

				if (isEndOfDirectory(thisFileData)) {
					return -1;
				}

				// if this is not a volume lable, long name, or deleted entry
				if (!isVolumeLabel(thisFileData) && !isLongName(thisFileData) && !isEmptyDirectoryEntry(thisFileData)) {
					// check to see if the current directory matches our search and return byte address of entry if it does
					//TODO: fix this
					/**
					FIX THIS strlen or strcmp
					*/


					name = strtok(thisFileData.fileName, " ");
					if (isFile(thisFileData))
					{
						ext = strtok(NULL, " ");
					}

					strcpy(toCompare, name);
					if (isFile(thisFileData))
					{
						strcat(toCompare, ".");
						strcat(toCompare, ext);
					}

					for (i = 0; i < SH_DIRNAME_SIZE + 1; i++)
					{
						toCompare[i] = tolower(toCompare[i]);
					}

					if (strcmp(toCompare, fileName) == 0)
					{
						return currentByteAddress;
					}	  	
				}
			}
		}
			
	
	
		// determine the next currentClusterNumber
		thisFATEntry = getFATEntry(filePtr, thisBootSector, currentClusterNumber);
		currentClusterNumber = thisFATEntry.nextClusterNumber;
	}
}

uint32_t getFirstClusterOfEntry(bootSector thisBootSector, fileData thisFileData) {
	
	uint32_t firstClusterEntry;

	//Get the high word from the file and shift it left 16 bits
	firstClusterEntry = thisFileData.firstClusterNumHI;
	firstClusterEntry = firstClusterEntry << 16;

	//Get the low word from the file and add it to the first cluster entry
	firstClusterEntry += thisFileData.firstClusterNumLO;

	return firstClusterEntry;
}

time getCreateTime(fileData thisFileData) {
	time thisTime;
	thisTime = convertToTimeStruct(thisFileData.createTime);
	return thisTime;
}

date getCreateDate(fileData thisFileData) {
	date thisDate;
	thisDate = convertToDateStruct(thisFileData.createDate);
	return thisDate;
}

date getLastAccessDate(fileData thisFileData) {
	date thisDate;
	thisDate = convertToDateStruct(thisFileData.lastAccessDate);
	return thisDate;
}

time getWriteTime(fileData thisFileData) {
	time thisTime;
	thisTime = convertToTimeStruct(thisFileData.writeTime);
	return thisTime;
}

date getWriteDate(fileData thisFileData) {
	date thisDate;
	thisDate = convertToDateStruct(thisFileData.writeDate);
	return thisDate;
}

date convertToDateStruct(uint16_t machineRepresentation) {
	date thisDate; 
	uint16_t mask;

	// 1. make a bit mask for bits 5-15
	mask = ~(~0 << 5);

	// 2. take bitwise AND of this and the machine represenation
	thisDate.day = mask & machineRepresentation;

	// 3. shift 5 bits to the right to put month in the lowest order
	machineRepresentation = machineRepresentation >> 5;

	// 4. make a bit mask for bits 4-15
	mask = ~(~0 << 4);

	// 5. take bitwise AND of this and the machine represenation
	thisDate.month = mask & machineRepresentation;

	// 6. shift 4 bits to the right to put year in lowest order
	machineRepresentation = machineRepresentation >> 4;

	// 7. make a bit mask for 7-15 
	mask = ~(~0 << 7);

	// 8. take bitwise AND of this and the machine representation
	thisDate.year = mask & machineRepresentation;

	// 9. add epoch to the calculated year
	thisDate.year += EPOCH_YEAR;

	return thisDate;
}

time convertToTimeStruct(uint16_t machineRepresentation) {
	time thisTime;
	uint16_t mask;

	// 1. make a bit mask for bits 5-15
	mask = ~(~0 << 5);
	
	// 2. take a bitwise AND of this and the machine representation
	thisTime.seconds = mask & machineRepresentation;

	// 3. multiply seconds by 2 (because granularity is 2 seconds)
	thisTime.seconds *= 2;

	// 4. shift 5 bits to the right to put minutes in lowest order
	machineRepresentation = machineRepresentation >> 5;

	// 5. make a bit mask for 6-15
	mask = ~(~0 << 6);

	// 6. take a bitwise AND of this and the machine representation
	thisTime.minutes = mask & machineRepresentation;

	// 7. shift 6 bits to the right to put hours in the lowest order
	machineRepresentation = machineRepresentation >> 6;

	// 8. make a bit mask for 5-15
	mask = ~(~0 << 5);

	// 9. take a bitwise AND of this and the machine representation
	thisTime.hours = mask & machineRepresentation;

	return thisTime;
}




