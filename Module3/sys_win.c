#include <windows.h>

int isRunning = 1;

int BufferWidth = 640;
int BufferHeight = 480;
// 4 means 32 bit RGB mode
// 1 means 8 bit palletized mode
int BytesPerPixel = 1;
void* BackBuffer;


// So we can have more than 1 element in the pallet array
typedef struct dibinfo_s {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD acolors[256];
} dibinfo_t;

dibinfo_t BitMapInfo = { 0 };

void drawRect(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char* buffer) {
	
	unsigned int color = ((r << 16) | (g << 8) | b);

	// Check bounds
	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}
	if ((x + w) > BufferWidth) {
		w = BufferWidth - x;
	}
	if ((y + h) > BufferHeight) {
		h = BufferHeight - y;
	}
	
	// move to first pixel
	buffer += (BufferWidth * BytesPerPixel * y) + (x * BytesPerPixel);

	int* bufferWalker = (int*)buffer;
	for (int height = 0; height < h; ++height) {
		for (int width = 0; width < w; ++width) {
			*bufferWalker++ = color;
		}

		buffer += BufferWidth * BytesPerPixel;
		bufferWalker = (int*)buffer;
	}

}

// 8 bit version
// Color is index into pallet
void drawRect8(int x, int y, int w, int h, unsigned char color, unsigned char* buffer) {
	
	// Check bounds
	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}
	if ((x + w) > BufferWidth) {
		w = BufferWidth - x;
	}
	if ((y + h) > BufferHeight) {
		h = BufferHeight - y;
	}

	// move to first pixel
	buffer += (BufferWidth * BytesPerPixel * y) + (x * BytesPerPixel);

	unsigned char* bufferWalker = buffer;
	for (int height = 0; height < h; ++height) {
		for (int width = 0; width < w; ++width) {
			*bufferWalker++ = color;
		}

		buffer += BufferWidth * BytesPerPixel;
		bufferWalker = buffer;
	}
}

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
			"Lesson 3.3", dwStyle,
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
	BitMapInfo.bmiHeader.biHeight = -BufferHeight;
	BitMapInfo.bmiHeader.biWidth = BufferWidth;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 8 * BytesPerPixel;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;

	BackBuffer = malloc(BufferWidth * BufferHeight * BytesPerPixel);

	// Initialize the pallet
	if (BytesPerPixel == 1) {
		BitMapInfo.acolors[0].rgbRed = 0;
		BitMapInfo.acolors[0].rgbGreen = 0;
		BitMapInfo.acolors[0].rgbBlue = 0;

		for (int i = 1; i < 256; i++) {
			BitMapInfo.acolors[i].rgbRed = rand() % 256;
			BitMapInfo.acolors[i].rgbGreen = rand() % 256;
			BitMapInfo.acolors[i].rgbBlue = rand() % 256;
		}
	}


	MSG msg;

	while (isRunning) {

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (BytesPerPixel == 4) {
			//Pushing 4 bytes at a time
			int* memoryWalker = (int*)BackBuffer;
			for (int height = 0; height < BufferHeight; ++height) {
				for (int width = 0; width < BufferWidth; ++width) {
					
					unsigned char red = rand() % 256;
					unsigned char green = rand() % 256;
					unsigned char blue = rand() % 256;
					
					// Little endian memory layout
					// But RGB is layed out in that order in memory, 8 bit each
					*memoryWalker++ = ((red << 16) | (green << 8) | blue);
				}
			}
			drawRect(10, 10, 300, 200, 255, 0, 0, BackBuffer);
		}
		else if (BytesPerPixel == 1) {
			// 8 bit version
			unsigned char* memoryWalker = BackBuffer;
			for (int height = 0; height < BufferHeight; ++height) {
				for (int width = 0; width < BufferWidth; ++width) {

					*memoryWalker++ = rand() % 256;
				}
			}

			
			drawRect8(10, 10, 300, 150, 1, BackBuffer);
		}

		// Get device context
		// Abstraction to get window state to draw stuff to it
		HDC dc = GetDC(MainWindow);

		StretchDIBits(dc, 
			0, 0, BufferWidth, BufferHeight, 
			0, 0, BufferWidth, BufferHeight, 
			BackBuffer, (BITMAPINFO*)&BitMapInfo, DIB_RGB_COLORS, SRCCOPY);

		DeleteObject(dc);

	}


	return EXIT_SUCCESS;
};