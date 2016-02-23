#include "quakedef.h"
#include "winquake.h"

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

	return 0;
}