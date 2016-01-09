#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_ARGVS 50

int argc = 1;
const char *largv[MAX_NUM_ARGVS];

/**
	Returns the largv index of the provided parameter string
*/
int COM_CheckParm(char *parm) {

	for (int i = 1; i < argc; i++) {
		if (!strcmp(parm, largv[i])) {
			return i;
		}
	}
	
	return 0;
}

/*
	lpCmdLine: command line args as a single string with program name and leading whitespace excluded
*/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// Initialize first element in arg array to 0
	largv[0] = "";
	
	
	while (*lpCmdLine && (argc < MAX_NUM_ARGVS)) {
		// Skip any invalid characters
		while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126))) {
			lpCmdLine++;
		}

		// At the start of each word, save start pointer in largv and clear out lpCmdLine
		if (*lpCmdLine) {
			largv[argc] = lpCmdLine;
			argc++;

			while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126))) {
				lpCmdLine++;
			}

			if (*lpCmdLine) {
				*lpCmdLine = 0;
				lpCmdLine++;
			}
		}
	}

	// Add a 0 to the end of command line args array
	largv[argc] = "";

	int test = COM_CheckParm("-setalpha");
//	int value = atoi(*largv[3]);

	return 0;
}