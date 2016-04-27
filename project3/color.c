/////////////////////////////////////////////////////////////////////////////////
// INCLUDES /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#include "color.h"

/////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
void printColor(char* colorString, char* outputString, FILE* file) {
	char* color;		// the ANSI string code for the indicated color;
	
	if (strncmp(colorString, "red", 3) == 0) {
		color = ANSI_COLOR_RED;
	}
	else if (strncmp(colorString, "yellow", 6) == 0) {
		color = ANSI_COLOR_YELLOW;
	}
	else if (strncmp(colorString, "green", 5) == 0) {
		color = ANSI_COLOR_GREEN;
	}
	else if (strncmp(colorString, "magenta", 7) == 0) {
		color = ANSI_COLOR_MAGENTA;
	}
	else if (strncmp(colorString, "cyan", 4) == 0) {
		color = ANSI_COLOR_CYAN;
	}
	else if (strncmp(colorString, "blue", 4) == 0) {
		color = ANSI_COLOR_BLUE;
	}
	else {
		color = ANSI_COLOR_RESET;
	}

	fputs(color, file);
	fputs(outputString, file);
	colorReset(file);
}

void printColorTemplate(char* templateString, char* outputString, FILE* file) {
	int i; 					// looping variable
	int colorIndex = 0;		// keeps track of the current color we are going to color the character
	int colorNumber;		// keeps track of how many colors are in the colors array
	char* colors[7];		// contains a repeating color pattern of up to 7 colors

	if (strncmp(templateString, "rainbow", 7) == 0) {	
		colors[0] = ANSI_COLOR_RED;	
		colors[1] = ANSI_COLOR_GREEN;	
		colors[2] = ANSI_COLOR_YELLOW;	
		colors[3] = ANSI_COLOR_BLUE;	
		colors[4] = ANSI_COLOR_MAGENTA;	
		colors[5] = ANSI_COLOR_CYAN;	
		colorNumber = 6;	
	}

	if (strncmp(templateString, "panther", 7) == 0) {	
		colors[0] = ANSI_COLOR_MAGENTA;	
		colors[1] = ANSI_COLOR_YELLOW;		
		colorNumber = 2;	
	}
		

	// loops through all characters and colors to print out the color template
	for (i = 0; i < strlen(outputString); i++) {
		fputs(colors[colorIndex], file);
		fputc(outputString[i], file);

		colorIndex = (colorIndex + 1) % (colorNumber);
	}
	
	// reset the output of the shell to white
	// need this unless you want your shell to be a different color!
	colorReset(file);
}

void colorReset(FILE* file) {
	fputs(ANSI_COLOR_RESET, file);
}
