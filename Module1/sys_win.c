#include "winquake.h"
#include "quakedef.h"


/*
	lpCmdLine: command line args as a single string with program name and leading whitespace excluded
*/
int32 CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32 nCmdShow) {
	
	// This is pass by value. lpCmdLine is unchanged in this context
	COM_ParseCmdLine(lpCmdLine);

	// Tests
	int32 test = COM_CheckParm("-setalpha");
	int32 value = Q_atoi(com_argv[test+1]);

	return 0;
}