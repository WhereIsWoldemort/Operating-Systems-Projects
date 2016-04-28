//////////////////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "color.h"

//////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void testPrintColor(int testNumber);
void testPrintColorTemplate(int testNumber);
void testPrintFailure(int testNumber);
void testPrintSuccess(int testNumber);

//////////////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
int main() {
	testPrintColor(1);
	testPrintColor(2);
	testPrintColor(3);
	testPrintColor(4);
	testPrintColor(5);
	testPrintColor(6);
	testPrintColorTemplate(1);
	testPrintColorTemplate(2);
	testPrintFailure(1);
	testPrintSuccess(1);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void testPrintColor(int testNumber) {
	if (testNumber == 1) {
		printColor("red", "RED!!\n", stdout);
	}

	else if (testNumber == 2) {
		printColor("yellow", "YELLOW!!\n", stdout);
	}

	else if (testNumber == 3) {
		printColor("blue", "BLUE!!\n", stdout);
	}

	else if (testNumber == 4) {
		printColor("green", "GREEN!!\n", stdout);
	}

	else if (testNumber == 5) {
		printColor("magenta", "MAGENTA!!\n", stdout);
	}

	else if (testNumber == 6) {
		printColor("cyan", "CYAN!!\n", stdout);	
	}

}

void testPrintColorTemplate(int testNumber) {
	if (testNumber == 1) {
		printColorTemplate("rainbow", "poop rainbows!\n", stdout);
	}
	else if (testNumber == 2) {
		printColorTemplate("panther", "panther pride\n", stdout);
	} 
}

void testPrintFailure(int testNumber) {
	printFailure("This is supposed to be an error!\n");
}

void testPrintSuccess(int testNumber) {
	printSuccess("Hooray! We succeeded!\n");
}

