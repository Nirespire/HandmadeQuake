#include "quakedef.h"
#include "winquake.h"

static BOOL isRunning = TRUE;


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

void sys_shutdown() {
	isRunning = FALSE;
}

// Function who's pointer will be passed to Windows
// Windows uses this to pass messages to the program
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	LRESULT result = 0;

	switch (uMsg) {

		// When window loses focus
	case WM_ACTIVATE:
		break;

		// When window is closed
	case WM_DESTROY:
		sys_shutdown();
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


	// Default window procedure.
	// Return default response for any message we don't care about
	return result;
}


/*
lpCmdLine: command line args as a single string with program name and leading whitespace excluded
*/
int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	// This is pass by value. lpCmdLine is unchanged in this context
	COM_ParseCmdLine(lpCmdLine);

	// Init a window class with all 0
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "Module 2";

	if (!RegisterClass(&wc)) {
		exit(EXIT_FAILURE);
	}

	HWND mainWindow;
	DWORD WindowStyle = WS_OVERLAPPEDWINDOW;

	RECT r;
	r.top = r.left = 0;
	r.right = 800;
	r.bottom = 600;

	// Use this to find the actual, active space in the window
	AdjustWindowRect(&r, WindowStyle, FALSE);

	mainWindow = CreateWindowEx(
		0,
		"Module 2",
		"Lesson 2.6",
		WindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		0);

	ShowWindow(mainWindow, SW_SHOWDEFAULT);

	// Way to look at the state of the screen
	HDC DeviceContext = GetDC(mainWindow);
	// Paint the screen black
	// 0,0 is upper left corner
	PatBlt(DeviceContext, 0, 0, 800, 600, BLACKNESS);
	ReleaseDC(mainWindow, DeviceContext);

	// Call init code
	host_init();

	// Initialize the timer
	float oldtime = sys_initFloatTime();


	MSG msg;

	while (isRunning) {
		// Check with OS
		// If there is a message, return 1 and fills msg Else return 0
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// Translates msg to string
			TranslateMessage(&msg);

			// Sends it to the callback function MainWndProc
			// Returns a result of the message
			DispatchMessage(&msg);
		}

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