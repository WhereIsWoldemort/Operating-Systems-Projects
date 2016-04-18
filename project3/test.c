#include <stdio.h>
#include "booleans.h"

void testEndOfDirectory(int i);
int main() {
	testEndOfDirectory(3);
}

// tests: isEndOfDirectory 
// from: booleans.h
void testEndOfDirectory(int testNumber) {
	char fileName1[12] = "CHUCKLES";
	char fileName2[12] = "BANANA";
	char fileName3[12] = "\0";
	char* files[3] = {fileName1, fileName2, fileName3};

	int i;							// looping variable

	for (i = 0; i < 3; i++) {
		if (isEndOfDirectory(files[i])) {
			printf("HOLY CRAP! IT IS THE END!\n");
		}
		else {
			printf("Nope... it's not over yet.\n");
		}
	}
}

 
		

 
