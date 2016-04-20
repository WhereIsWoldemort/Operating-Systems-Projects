#include <stdio.h>
#include <sys/types.h>
#include "directory_entries.h"

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

/*uint32_t getFirstClusterOfEntry(fileData thisFileData) {
	
	uint32_t firstClusterEntry;

	//Get the high word from the file and shift it left 16 bits
	firstClusterEntry = thisFileData.firstClusterNumHI;
	firstClusterEntry = firstClusterEntry << 16;

	//Get the low word from the file and add it to the first cluster entry
	firstClusterEntry += thisFileData.firstClusterNumLO;

	//Convert the cluster num into the correct cluster num
	firstClusterEntry = getFirstSectorOfCluster(firstClusterEntry);

	//Convert the cluster num into a byte format
	firstClusterEntry = convertSectorNumToBytes(firstClusterEntry);	
}*/



