#include "directory_entries.h"
// In:			filename (char*)
// Out:			formattedFileName (char*)
// Purpose:		to get the first sector number of the data region
// Notes:
char* formatFileName(fileData thisFileData);

// In:			reservedSectorCount (uint16_t), numOfFATs (uint8_t), fatSize (uint32_t), rootDirectorySectors (uint32_t)
// Out:			formatterDirName (char*)
// Purpose:		to get the first sector number of the data region
// Notes:
//char* formatDirectoryName(char* directoryName);