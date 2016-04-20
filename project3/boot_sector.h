///////////////////////////////////////////////////////////////////////
// INCLUDES ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////
// MACROS /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#define READ_RESOLUTION	1		// the resolution we want to read at in bytes
#define BPS_OFFSET 		11		// bytes per sector 
#define BPS_SIZE 		2		
#define SPC_OFFSET 		13		// sectors per cluster
#define SPC_SIZE 		1		
#define RSC_OFFSET 		14		// reserved sector count
#define RSC_SIZE 		2		
#define NF_OFFSET 		16		// number of FATs
#define NF_SIZE			1
#define FS_OFFSET		36		// size of FAT
#define FS_SIZE			4
#define RC_OFFSET		44		// root cluster number
#define RC_SIZE			4		
#define REC_OFFSET		17		// root entry count
#define REC_SIZE		2
#define TS_OFFSET		32		// total sectors
#define TS_SIZE			4		

///////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
typedef struct {	
	uint16_t 	bytesPerSector;			// the number of bytes per sector
	uint8_t		sectorsPerCluster;		// the number of sectors per cluster
	uint16_t 	reservedSectorCount;	// the number of sectors in the reserved region
	uint8_t 	numOfFATs; 				// number of FATs
	uint32_t 	fatSize;				// the size of each FAT
	uint32_t 	rootClusterNum;			// the cluster number of the first cluster of the root
	uint16_t	rootEntryCount;			// the number of entries in the root directory (should be 0)
	uint32_t 	totalSectors;			// the total number of sectors in this volume
	uint32_t 	rootDirectorySectors; 	// number of sectors in the root directory
	uint32_t	firstDataSector;		// the first sector of data in the data region of the file system
} bootSector;

///////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// In:			filePtr (FILE*)	
// Out:			thisBootSector (bootSector (struct))
// Purpose:		to set up the bootSector data structure for our file system
// Notes:			
bootSector setUpBootSector(FILE* filePtr);


