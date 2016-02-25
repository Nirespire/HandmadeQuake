#include "quakedef.h"
#include "winquake.h"

static int BufferWidth = 640;
static int BufferHeight = 480;
static int WindowWidth = 640;
static int WindowHeight = 480;
// 4 means 32 bit RGB mode
// 1 means 8 bit palletized mode
static int BytesPerPixel = 4;
void* BackBuffer = NULL;

BITMAPINFO BitMapInfo = { 0 };
HWND MainWindow;

typedef enum {MS_WINDOWED, MS_FULLSCREEN} modestate_t;

typedef struct {
	modestate_t type;
	int width;
	int height;
} vmode_t;

vmode_t modeList[30];
int modenums = 0;

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
		//sys_shutdown();
		break;
	case WM_KEYDOWN: 
	{
		if (wParam == 'A') {
			VID_SetMode(0);
		}
		else if (wParam == 'S') {
			VID_SetMode(1);
		}
		else if (wParam == 'D') {
			VID_SetMode(2);
		}
		else if (wParam == 'F') {
			VID_SetMode(3);
		}
		else if (wParam == 'Q') {
			sys_shutdown();
		}
	} break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


	// Default window procedure.
	// Return default response for any message we don't care about
	return result;
}

void VID_InitWindowedMode(void) {
	modeList[modenums].type = MS_WINDOWED;
	modeList[modenums].width = 320;
	modeList[modenums].height = 240;
	++modenums;

	modeList[modenums].type = MS_WINDOWED;
	modeList[modenums].width = 640;
	modeList[modenums].height = 480;
	++modenums;

	modeList[modenums].type = MS_WINDOWED;
	modeList[modenums].width = 800;
	modeList[modenums].height = 600;
	++modenums;
		
	modeList[modenums].type = MS_WINDOWED;
	modeList[modenums].width = 1024;
	modeList[modenums].height = 768;
	++modenums;
}

void VID_InitFullscreenMode(void) {
	
}

void VID_SetMode(int modeIndex) {

	if (BackBuffer) {
		VID_Shutdown();
	}

	WindowWidth = modeList[modeIndex].width;
	WindowHeight = modeList[modeIndex].height;

	BufferHeight = WindowHeight;
	BufferWidth = WindowWidth;

	// TODO revisit fullscreen stuff
	//BOOL fullscreen = FALSE;
	//
	//if (fullscreen) {
	//	DEVMODE dmScreenSettings = { 0 };
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth = BufferWidth;
	//	dmScreenSettings.dmPelsHeight = BufferHeight;
	//	dmScreenSettings.dmBitsPerPel = 32;
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;
	//
	//	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
	//		dwExStyle = WS_EX_APPWINDOW;
	//		dwStyle = WS_POPUP;
	//	}
	//	else {
	//		fullscreen = FALSE;
	//	}
	//}

	DWORD windowExStyle = 0;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	// Create rectangle for window
	RECT r = { 0 };
	r.right = WindowWidth;
	r.bottom = WindowHeight;

	AdjustWindowRectEx(&r, windowStyle, 0, windowExStyle);


	// Create window
	MainWindow = CreateWindowEx(
		windowExStyle, "Module 3",
		"Lesson 3.5", windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		r.right - r.left, r.bottom - r.top,
		NULL, NULL,
		globalInstance, 0
		);

	ShowWindow(MainWindow, SW_SHOWDEFAULT);

	// Way to look at the state of the screen
	HDC DeviceContext = GetDC(MainWindow);
	// Paint the screen black
	// 0,0 is upper left corner
	PatBlt(DeviceContext, 0, 0, BufferWidth, BufferHeight, BLACKNESS);
	ReleaseDC(MainWindow, DeviceContext);

	// define our bitmap info
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biHeight = -BufferHeight;
	BitMapInfo.bmiHeader.biWidth = BufferWidth;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 8 * BytesPerPixel;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	BackBuffer = malloc(BufferWidth * BufferHeight * BytesPerPixel);

}


void VID_Init(void) {

	// Register a window class with all 0
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = globalInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "Module 3";

	if (!RegisterClassEx(&wc)) {
		exit(EXIT_FAILURE);
	}

	VID_InitWindowedMode();
	VID_InitFullscreenMode();

	VID_SetMode(0);

}

void VID_Update(void) {

	HDC dc = GetDC(MainWindow);

	StretchDIBits(dc,
		0, 0, WindowWidth, WindowHeight,
		0, 0, BufferWidth, BufferHeight,
		BackBuffer, (BITMAPINFO*)&BitMapInfo, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(MainWindow, dc);

}

void VID_Shutdown(void) {
	DestroyWindow(MainWindow);
	free(BackBuffer);
	BackBuffer = NULL;
}