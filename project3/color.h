#ifndef COLOR_H
#define COLOR_H
////////////////////////////////////////////////////////////////////////////
// INCLUDES ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////
// MACROS //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define ANSI_COLOR_RED 			"\x1b[31m"
#define ANSI_COLOR_GREEN 		"\x1b[32m"
#define ANSI_COLOR_YELLOW 		"\x1b[33m"
#define ANSI_COLOR_BLUE 		"\x1b[34m"
#define ANSI_COLOR_MAGENTA 		"\x1b[35m"
#define ANSI_COLOR_CYAN 		"\x1b[36m"
#define ANSI_COLOR_RESET 		"\x1b[0m"

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// In:			colorString (string), outputString (string), file (FILE*)
// Out:			none
// Purpose:		to print a string in a single color
// Notes: 		
void printColor(char* colorString, char* outputString, FILE* file); 

// In:			templateString (string), outputString (string), file (FILE*)
// Out:			none
// Purpose:		to print out a string in many preset color combinations
// Notes: 
void printColorTemplate(char* templateString, char* outputString, FILE* file);

// In:			file (FILE*)
// Out:			none
// Purpose:		to reset the display of a file to white
// Notes:
void colorReset(FILE* file);

// In:			outputString (string)
// Out:			none
// Purpose:		to print a string in red to indicate error or failure
// Notes:		different implementation (do not have to declare stdout as file destination)
void printFailure(char* outputString);

// In:			outputString (string)
// Out:			none
// Purpose:		to print a string in green to indicate something good
// Notes:		different implementation (do not have to declare stdout as file destination)
void printSuccess(char* outputString);

#endif /* COLOR_H */
