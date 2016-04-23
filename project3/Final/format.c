#include <string.h>
#include <stdio.h>
#include "format.h"

char* formatFileName(fileData thisFileData) {
	char* name;
	char* ext;
	static char* toReturn;

	name = strtok(thisFileData.fileName, " ");
	if (isFile(thisFileData))
	{
		ext = strtok(NULL, " ");
	}

	strcpy(toReturn, name);
	if (isFile(thisFileData))
	{
		strcat(toReturn, ".");
		strcat(toReturn, ext);
	}

	return toReturn;

}

//char* formatDirectoryName(char* directoryName) {

//}