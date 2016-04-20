#include <stdint.h>

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

#define DN_OFFSET		0		// directory name
#define DN_SIZE			11
#define DA_OFFSET		11		// directory attributes
#define DA_SIZE			1	
#define DCT_OFFSET		14		// create time
#define DCT_SIZE		2
#define DCD_OFFSET		16		// create date 
#define DCD_SIZE		2
#define DLAD_OFFSET		18		// last access date
#define DLAD_SIZE		2
#define DFCH_OFFSET		20		// first cluster number high order word
#define DFCH_SIZE		2
#define DWT_OFFSET		22		// write time
#define DWT_SIZE		2
#define DWD_OFFSET		24		// write date
#define DWD_SIZE		2
#define DFCL_OFFSET		26		// first cluster number low order word
#define DFCL_SIZE		2
#define DFS_OFFSET		28		// file size
#define DFS_SIZE		4
#define SH_FILE_NAME	11		// short file name size in characters 

#define READ_RESOLUTION	1		// the resolution we want to read at in bytes
#define SH_DIRNAME_SIZE	11		// the number of characters in a short directory name
#define DIR_ENTRY_SIZE	32		// the size of each entry within a directory in bytes
#define EPOCH_YEAR 		1980	// the year that all years are calculated from in fat32	

/////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITONS //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

typedef struct {
	char 		fileName[SH_FILE_NAME+1];	 	// the name of the file
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
fileData getFileData(FILE* filePtr, uint64_t byteAddress);

// In:			thisFileData (fileData (struct))
// Out:			boolean
// Purpose:		to determine if the file is read only
// Notes:
BOOL isReadOnly(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is a system file
// Notes:	
BOOL isSystemFile(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is a hidden file/directory
// Notes:	
BOOL isHiddenFile(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is the volume label of the volume
// Notes:	
BOOL isVolumeLabel(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out: 		boolean
// Purpose:		to determine if the current file directory is part of a long name
// Notes:		fileAttributes is really a byte we are representing as an int
BOOL isLongName(fileData thisFileData);

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
// Out:			boolean
// Purpose:		to detemine if we have reached the end of the directory (no more entries left)
// Notes:
BOOL isEndOfDirectory(fileData thisFileData);

// In:			thisFileData (fileData (struct))
// Out:			boolean
// Purpose:		to determine if a given directory entry is empty 
// Notes: 		this applies to directories/files that were deleted
BOOL isEmptyDirectoryEntry(fileData thisFileData); 

// In:			filePtr (FILE*), directoryName (char*), directoryAddress (uint64_t)
// Out:			boolean
// Purpose:		to determine if a file exists in the current directory
// Notes: 		this applies to directories/files that were deleted
uint64_t checkFileExists(FILE* filePtr, char* directoryName, uint64_t directoryAddress);

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

// In:			machineRepresentation (uint16_t)
// Out: 		thisDate (date (struct))
// Purpose: 	to convert the machine representation of the date into a human readable date
// Notes:		specific bits are extracted through shifting and using & for masking
date convertToDateStruct(uint16_t machineRepresentation);

// In: 			machineRepresentation (uint16_t)
// Out:			thisTime (time (struct))
// Purpose: 	to convert the machine representation of a time to a human-readable version
// Notes:		same logic as convertToDateStruct
time convertToTimeStruct(uint16_t machineRepresentation);





