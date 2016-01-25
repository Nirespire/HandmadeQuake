#include<stdio.h>
#include <windows.h>

BOOL isRunning = TRUE;

// Function who's pointer will be passed to Windows
// Windows uses this to pass messages to the program
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	LRESULT result = 0;

	switch (uMsg) {
	// When any key is released
	case WM_KEYUP:
		isRunning = FALSE;
		result = 56;
		break;
	// When window loses focus
	case WM_ACTIVATE:
		break;
	// When window is closed
	case WM_DESTROY:
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


	// Default window procedure.
	// Return default response for any message we don't care about
	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

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
		"Lesson 2.3",
		WindowStyle,
		200,
		200,
		r.right-r.left,
		r.bottom-r.top,
		NULL,
		NULL,
		hInstance,
		0 );

	ShowWindow(mainWindow, SW_SHOWDEFAULT);

	// Way to look at the state of the screen
	HDC DeviceContext = GetDC(mainWindow);
	// Paint the screen black
	// 0,0 is upper left corner
	PatBlt(DeviceContext, 0, 0, 800, 600, BLACKNESS);
	ReleaseDC(mainWindow, DeviceContext);

	// Union data structure to hold ticks returned by the OS
	LARGE_INTEGER frequency;
	// Returns ticks per second
	QueryPerformanceCounter(&frequency);

	// Compute seconds per tick
	double secondsPerTick = 1.0 / (double)frequency.QuadPart;

	LARGE_INTEGER tick, tock;
	// Returns ticks since boot
	QueryPerformanceCounter(&tick);

	MSG msg;
	LRESULT result;

	while (isRunning) {
		// Check with OS

		// If there is a message, return 1 and fills msg
		// Else return 0
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			// Translates msg to string
			TranslateMessage(&msg);
			// Sends it to the callback function MainWndProc
			// Returns a result of the message
			result = DispatchMessage(&msg);
		}

		// Update game if it needs to
		// Draw graphics if it's time to

		// Update timers
		QueryPerformanceCounter(&tock);
		__int64 interval = tock.QuadPart - tick.QuadPart;
		double secondsGoneBy = (double) interval * secondsPerTick;

		char buf[64];
		sprintf_s(buf, 64, "Total time: %3.7f \n", secondsGoneBy);
		OutputDebugString(buf);

		QueryPerformanceCounter(&tick);
	}
		
	return 0;
}