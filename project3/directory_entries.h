////////////////////////////////////////////////////////////////////////////
// INCLUDES ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include <sys/types.h>

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


/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITONS //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

typedef struct {
	char 		fileName[SH_DIRNAME_SIZE+1]; 	// the name of the file
	uint8_t		fileAttributes;					// all attributes associated with file
	uint16_t	createTime;						// time file was created
	uint16_t	createDate;						// date file was created
	uint16_t	lastAccessDate;					// date file was last accessed
	uint16_t	firstClusterNumHI;				// the higher value word of the first cluster number
	uint16_t	writeTime;						// time file was last written to
	uint16_t	writeDate;						// date file was last written to
	uint16_t	firstClusterNumLO;				// the lower value word of the first cluster number
	uint32_t	fileSize;						// the size of the file in bytes
	uint64_t	byteAddress;					// holds the byte address of the directory entry structure
} fileData;

typedef struct {
	int day;
	int month;
	int year;
} date;

typedef struct {
	int hours;
	int minutes;
	int seconds;
} time;

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// In: 		byteAddress (uint64_t)
// Out: 	thisFileData (fileData (struct))
// Purpose: to place all file metadata based on a byte address in a struct and return it
// Note: 	handles file metadata blocks that are 32 bits (4 bytes) in size
fileData getFileData(uint64_t byteAddress);

// In:			thisFileData (fileData (struct))
// Out:			boolean
// Purpose:		to detemine if we have reached the end of the directory (no more entries left)
// Notes:
BOOL isEndOfDirectory(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out:			boolean
// Purpose:		to determine if a given directory entry is empty 
// Notes: 		this applies to directories/files that were deleted
BOOL isEmptyDirectoryEntry(fileData thisFileData); 

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is part of a long name
// Notes:		fileAttributes is really a byte we are representing as an int
BOOL hasLongName(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is a directory
// Notes:	
BOOL isDirectory(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is a file
// Notes:	
BOOL isFile(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is the volume label of the volume
// Notes:	
BOOL isVolumeLabel(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is a file
// Notes:		fileAttributes is really a byte we are representing as an int
uint32_t getFirstClusterOfEntry(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		thisTime (time (struct))
// Purpose:		to get the create time of the directory entry
// Notes: 
time getCreateTime(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		thisDate (date (struct))
// Purpose:		to get the create date of the directory entry
// Notes: 
date getCreateDate(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		thisDate (date (struct))
// Purpose:		to get the last access date of the directory entry
// Notes: 
date getLastAccessDate(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		thisTime (time (struct))
// Purpose:		to get the last write time of the directory entry
// Notes: 
time getWriteTime(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		thisDate (date (struct))
// Purpose:		to get the last write date of the directory entry
// Notes: 
date getWriteDate(fileData thisFileData); 

// In:			thisFileData (fileData (struct))
// Out: 		thisDate (date (struct))
// Purpose: 	to convert the machine representation of the date into a human readable date
// Notes:		specific bits are extracted through shifting and using & for masking
date convertToDateStruct(uint16_t machineRepresentation);

// In: 			machineRepresentation (uint16_t)
// Out:			thisTime (time (struct))
// Purpose: 	to convert the machine representation of a time to a human-readable version
// Notes:		same logic as convertToDateStruct
time convertToTimeStruct(uint16_t machineRepresentation);
