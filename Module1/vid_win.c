#include "quakedef.h"
#include "winquake.h"

// These are read in from the OS through devmode now in the init functions
static int BufferWidth = 0;
static int BufferHeight = 0;
static int WindowWidth = 0;
static int WindowHeight = 0;
// 4 means 32 bit RGB mode
// 1 means 8 bit palletized mode
static int BytesPerPixel = 4;
void* BackBuffer = NULL;

BITMAPINFO BitMapInfo = { 0 };
HWND MainWindow;

typedef enum {MS_WINDOWED, MS_FULLSCREEN} modestate_t;

typedef struct {
	modestate_t type;
	int32 width;
	int32 height;
	uint32 Hz;
} vmode_t;

vmode_t modeList[40];
int32 ModeCount = 0;
int32 FirstFullscreenMode = -1;

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
		else if (wParam == '1') {
			VID_SetMode(FirstFullscreenMode);
		}
		else if (wParam == '2') {
			VID_SetMode(FirstFullscreenMode + 1);
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
	modeList[ModeCount].type = MS_WINDOWED;
	modeList[ModeCount].width = 320;
	modeList[ModeCount].height = 240;
	modeList[ModeCount].Hz = 0;
	++ModeCount;

	modeList[ModeCount].type = MS_WINDOWED;
	modeList[ModeCount].width = 640;
	modeList[ModeCount].height = 480;
	modeList[ModeCount].Hz = 0;
	++ModeCount;

	modeList[ModeCount].type = MS_WINDOWED;
	modeList[ModeCount].width = 800;
	modeList[ModeCount].height = 600;
	modeList[ModeCount].Hz = 0;
	++ModeCount;
		
	modeList[ModeCount].type = MS_WINDOWED;
	modeList[ModeCount].width = 1024;
	modeList[ModeCount].height = 768;
	modeList[ModeCount].Hz = 0;
	++ModeCount;
}

void VID_InitFullscreenMode(void) {

	FirstFullscreenMode = ModeCount;

	DEVMODE devMode;
	int modeNum = 0;
	BOOL success;
	int oldWidth = 0, oldHeight = 0;

	// Get all resolutions supported by the display
	do {
		success = EnumDisplaySettings(NULL, modeNum, &devMode);

		if (devMode.dmPelsHeight == oldHeight && devMode.dmPelsWidth == oldWidth) {
			
			// For similar frequencies, get the one with the highest refersh rate
			if (devMode.dmDisplayFrequency > modeList[ModeCount - 1].Hz) {
				modeList[ModeCount - 1].Hz = devMode.dmDisplayFrequency;
			}
			
		}
		else {
			modeList[ModeCount].type = MS_FULLSCREEN;
			modeList[ModeCount].width = devMode.dmPelsWidth;
			modeList[ModeCount].height = devMode.dmPelsHeight;
			modeList[ModeCount].Hz = devMode.dmDisplayFrequency;
			++ModeCount;

			oldHeight = devMode.dmPelsHeight;
			oldWidth = devMode.dmPelsWidth;
		}

		++modeNum;

	} while (success);
}

void VID_SetWindowedMode(int modeIndex) {
	
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
}

void VID_SetFullscreenMode(int modeIndex) {
	
	BOOL fullscreen = TRUE;

	DWORD windowExStyle = 0;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	
	if (fullscreen) {
		DEVMODE dmScreenSettings = { 0 };
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = modeList[modeIndex].width;
		dmScreenSettings.dmPelsHeight = modeList[modeIndex].height;
		dmScreenSettings.dmDisplayFrequency = modeList[modeIndex].Hz;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;
	
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
			windowExStyle = WS_EX_APPWINDOW;
			windowStyle = WS_POPUP;
		}
		else {
			fullscreen = FALSE;
		}
	}

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

	SetWindowLong(MainWindow, GWL_STYLE, 0);
}


void VID_SetMode(int modeIndex) {

	if (BackBuffer) {
		
		VID_Shutdown();
	}

	WindowWidth = modeList[modeIndex].width;
	WindowHeight = modeList[modeIndex].height;

	BufferHeight = WindowHeight;
	BufferWidth = WindowWidth;

	if (modeList[modeIndex].type == MS_WINDOWED) {
		VID_SetWindowedMode(modeIndex);
	}
	else {
		VID_SetFullscreenMode(modeIndex);
	}

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
	ChangeDisplaySettings(NULL, 0);
	DestroyWindow(MainWindow);
	free(BackBuffer);
	BackBuffer = NULL;
}