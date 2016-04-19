////////////////////////////////////////////////////////////////////////////
// MACROS //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef BOOL 
	#define BOOL int
#endif 

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#include <sys/types.h>

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


// In:
// Out:
// Purpose:
// Notes:
BOOL isEndOfDirectory(char* fileName);

// In:		fileAttributes (int)
// Out: 	boolean
// Purpose:	to determine if the current file directory is part of a long name
// Notes:	fileAttributes is really a byte we are representing as an int
BOOL hasLongName(uint8_t fileAttributes);

// In:		fileAttributes (int)
// Out: 	boolean
// Purpose:	to determine if the current file directory is a directory
// Notes:	fileAttributes is really a byte we are representing as an int
BOOL isDirectory(uint8_t fileAttributes);

// In:		fileAttributes (int)
// Out: 	boolean
// Purpose:	to determine if the current file directory is a file
// Notes:	fileAttributes is really a byte we are representing as an int
BOOL isFile(uint8_t fileAttributes);
