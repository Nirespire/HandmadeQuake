#include "quakedef.h"
#include "winquake.h"
#include <stdio.h>

static BOOL isRunning = TRUE;
HINSTANCE globalInstance;


/***************
*	TIMER CODE *
****************/

static double GTimePassed = 0;
static double secondsPerTick = 0;
static __int64 GTimeCount = 0;

float sys_initFloatTime(void) {
	// Union data structure to hold ticks returned by the OS
	LARGE_INTEGER frequency;

	// Returns ticks per second
	QueryPerformanceFrequency(&frequency);

	// Compute seconds per tick
	secondsPerTick = 1.0 / (double)frequency.QuadPart;

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	GTimeCount = counter.QuadPart;

	return 0;
}

float sys_floatTime(void) {
	LARGE_INTEGER counter;

	// Returns ticks since boot
	QueryPerformanceCounter(&counter);


	__int64 interval = counter.QuadPart - GTimeCount;
	GTimeCount = counter.QuadPart;
	double secondsGoneBy = (double)interval * secondsPerTick;
	GTimePassed += secondsGoneBy;

	return (float)GTimePassed;
}

/*******************************/

/***************
*	FILE CODE *
****************/
#define MAX_HANDLES 10
static FILE* FileHandles[MAX_HANDLES] = { 0 };

static int findHandle(void) {
	for (int i = 0; i < MAX_HANDLES; ++i) {
		if (FileHandles[i] == 0) {
			return i;
		}
	}

	return -1;
}

static int fileLength(FILE *f) {
	int pos;
	int end;

	pos = ftell(f);

	fseek(f, 0, SEEK_END);

	end = ftell(f);

	fseek(f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead(char *path, int *size) {
	int handleIdx = findHandle();

	FILE* fileHandle = FileHandles[handleIdx];

	fopen_s(&fileHandle, path, "rb");

	if (fileHandle == 0) {
		if (size != 0) {
			*size = -1;
		}
		return -1;
	}

	FileHandles[handleIdx] = fileHandle;

	if (size != 0) {
		*size = fileLength(fileHandle);
	}

	return handleIdx;
}

int Sys_FileOpenWrite(char *path) {
	int handleIdx = findHandle();

	FILE* fileHandle = FileHandles[handleIdx];

	fopen_s(&fileHandle, path, "wb");


	if (fileHandle == 0) {
		handleIdx = -1;
	}

	FileHandles[handleIdx] = fileHandle;

	return handleIdx;
}

void Sys_FileClose(int handle) {
	if (handle < 0 || handle > MAX_HANDLES) {
		return;
	}

	if (FileHandles[handle] != NULL) {
		fclose(FileHandles[handle]);
		FileHandles[handle] = NULL;
	}
}

void Sys_FileSeek(int handle, int pos) {
	if (handle < 0 || handle > MAX_HANDLES) {
		return;
	}

	if (FileHandles[handle] != NULL) {
		fseek(FileHandles[handle], pos, SEEK_SET);
	}
}

int Sys_FileRead(int handle, void* dest, int count) {
	if (handle < 0 || handle > MAX_HANDLES || dest == 0) {
		return 0;
	}

	// This cast assumes not more than 32GB read at once
	int bytesRead = (int)fread_s(dest, count, 1, count, FileHandles[handle]);

	return bytesRead;
}

int Sys_FileWrite(int handle, void* source, int count) {
	if (handle < 0 || handle > MAX_HANDLES || source == 0) {
		return 0;
	}

	int bytesWritten = (int)fwrite(source, 1, count, FileHandles[handle]);

	return bytesWritten;
}

/*******************************/

void sys_sendKeyEvents(void) {
	MSG msg;
	// Check with OS
	// If there is a message, return 1 and fills msg Else return 0
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		// Translates msg to string
		TranslateMessage(&msg);

		// Sends it to the callback function MainWndProc
		// Returns a result of the message
		DispatchMessage(&msg);
	}
}

void sys_shutdown(void) {
	isRunning = FALSE;
}

/*
lpCmdLine: command line args as a single string with program name and leading whitespace excluded
*/
int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	globalInstance = hInstance;

	// This is pass by value. lpCmdLine is unchanged in this context
	COM_ParseCmdLine(lpCmdLine);

	// Call init code
	host_init();

	// Initialize the timer
	float oldtime = sys_initFloatTime();


	while (isRunning) {


		// Get a new value from the initialized timer
		float newtime = sys_floatTime();

		// Separate out update and rendering logic
		host_frame(newtime - oldtime);

		// Better to use subtractive tracking of time
		// No lost frames on each iteration due to floating point precision
		oldtime = newtime;


		/*
		// DEBUG
		char buf[64];
		sprintf_s(buf, 64, "Total time: %3.7f \n", newtime);
		OutputDebugString(buf);
		*/


	}

	// Shutdown code
	host_shutdown();

#if _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}