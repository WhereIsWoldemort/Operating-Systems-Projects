#include <stdio.h>
#include <sys/types.h>
#include "booleans.h"

BOOL isEndOfDirectory(char* fileName) {
	if (fileName[0] == '\0') {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
