#include <windows.h>

// Function who's pointer will be passed to Windows
// Windows uses this to pass messages to the program
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Default window procedure.
	// Return default response for any message we don't care about
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
		"Lesson 2.1",
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
		
	return 0;
}