#include <windows.h>

int isRunning = 1;

int BufferWidth = 640;
int BufferHeight = 480;
void* BackBuffer;

BITMAPINFO BitMapInfo = { 0 };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (uMsg) {

	case WM_CLOSE:
	case WM_DESTROY:
	case WM_KEYUP:
		isRunning = 0;
		break;

	default:
		result = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return result;
}

int CALLBACK WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	// Define window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "Module 3";

	RegisterClassExA(&wc);

	DWORD dwExStyle = 0;
	DWORD dwStyle = WS_OVERLAPPED;

	BOOL fullscreen = FALSE;

	if (fullscreen) {
		DEVMODE dmScreenSettings = { 0 };
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = BufferHeight;
		dmScreenSettings.dmPelsWidth = BufferWidth;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
		}
		else {
			fullscreen = FALSE;
		}
	}

	// Create rectangle for window
	RECT r = { 0 };
	r.right = BufferWidth;
	r.bottom = BufferHeight;

	AdjustWindowRectEx(&r, dwStyle, 0, dwExStyle);

	// Create window
	HWND MainWindow = CreateWindowEx(
			dwExStyle, "Module 3",
			"Lesson 3.2", dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			r.right - r.left, r.bottom - r.top,
			NULL, NULL,
			hInstance, 0
		);

	if (fullscreen) {
		SetWindowLong(MainWindow, GWL_STYLE, 0);
	}

	ShowWindow(MainWindow, nShowCmd);

	// define out bitmap info
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biHeight = BufferHeight;
	BitMapInfo.bmiHeader.biWidth = BufferWidth;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	BackBuffer = malloc(BufferWidth * BufferHeight * 4);

	MSG msg;

	while (isRunning) {

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Pushing 4 bytes at a time
		int* memoryWalker = (int*)BackBuffer;
		for (int height = 0; height < BufferHeight; ++height) {
			for (int width = 0; width < BufferWidth; ++width) {
				
				char red = rand() % 256;
				char green = rand() % 256;
				char blue = rand() % 256;
				
				// Little endian memory layout
				// But RGB is layed out in that order in memory, 8 bit each
				*memoryWalker++ = ((red << 16) | (green << 8) | blue);
			}
		}

		// Get device context
		// Abstraction to get window state to draw stuff to it
		HDC dc = GetDC(MainWindow);

		StretchDIBits(dc, 
			0, 0, BufferWidth, BufferHeight, 
			0, 0, BufferWidth, BufferHeight, 
			BackBuffer, &BitMapInfo, DIB_RGB_COLORS, SRCCOPY);

		DeleteObject(dc);

	}


	return EXIT_SUCCESS;
};