#include <stdio.h>
#include <sys/types.h>
#include "directory_entries.h"

BOOL isEndOfDirectory(fileData thisFileData) {
	if (thisFileData.fileName[0] == '\0') {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isEmptyDirectoryEntry(fileData thisFileData) {	
	if (thisFileData.fileName[0] == 5) {
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

BOOL isReadOnly(fileData thisFileData) {
	if ((thisFileData.fileAttributes & 1) == 1) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL isVolumeLabel(fileData thisFileData) {
	if ((thisFileData.fileAttributes & 8) == 8) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
