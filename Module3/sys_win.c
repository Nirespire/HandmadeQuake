#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

int isRunning = 1;

int BufferWidth = 640;
int BufferHeight = 480;
int WindowWidth = 640;
int WindowHeight = 480;
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

void drawPic8(int x, int y, int width, int height, unsigned char* source, unsigned char* dest) {
	
	// move to first pixel
	dest += (BufferWidth * BytesPerPixel * y) + (x * BytesPerPixel);

	unsigned char* bufferWalker = dest;

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			*bufferWalker = *source;
			++bufferWalker;
			++source;
		}

		dest += BufferWidth * BytesPerPixel;
		bufferWalker = dest;
	}
}

void drawPic32(int x, int y, int width, int height, unsigned char* source, unsigned char* dest) {
	// move to first pixel
	dest += (BufferWidth * BytesPerPixel * y) + (x * BytesPerPixel);

	unsigned int* bufferWalker = (unsigned int*)dest;

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {

			// Pull out the RGB values from the pallet and draw them directly to the object
			int* colorValue = (int*)&BitMapInfo.acolors[*source];

			*bufferWalker = *colorValue;
			++bufferWalker;
			++source;
		}

		dest += BufferWidth * BytesPerPixel;
		bufferWalker = (unsigned int*)dest;
	}
}

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

	// Create rectangle for window
	RECT r = { 0 };
	r.right = WindowWidth;
	r.bottom = WindowHeight;

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

	// TODO
	//if (fullscreen) {
	//	SetWindowLong(MainWindow, GWL_STYLE, 0);
	//}

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
	// This will only be used in 8 bit mode
	FILE *pallete = fopen("palette.lmp", "r");
	void* rawData = malloc(256 * 3);
	unsigned char* palleteData = rawData;
	size_t retVal = fread(palleteData, 1, 768, pallete);

	for (int i = 0; i < 256; i++) {
		BitMapInfo.acolors[i].rgbRed = *palleteData++;
		BitMapInfo.acolors[i].rgbGreen = *palleteData++;
		BitMapInfo.acolors[i].rgbBlue = *palleteData++;
	}

	free(rawData);
	fclose(pallete);
	

	// Load some assets
	FILE *disc = fopen("DISC.lmp", "r");
	int discWidth, discHeight;
	retVal = fread(&discWidth, 1, 4, disc);
	retVal = fread(&discHeight, 1, 4, disc);
	void* discData = malloc(discWidth * discHeight);
	retVal = fread(discData, 1, discWidth * discHeight, disc);
	fclose(disc);

	FILE *pause = fopen("pause.lmp", "r");
	int pauseWidth, pauseHeight;
	retVal = fread(&pauseWidth, 1, 4, pause);
	retVal = fread(&pauseHeight, 1, 4, pause);
	void* pauseData = malloc(pauseWidth * pauseHeight);
	retVal = fread(pauseData, 1, pauseWidth * pauseHeight, pause);
	fclose(pause);


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
			//drawRect(10, 10, 300, 200, 255, 0, 0, BackBuffer);

			drawPic32(10, 10, discWidth, discHeight, discData, BackBuffer);
			drawPic32(100, 100, pauseWidth, pauseHeight, pauseData, BackBuffer);
		}
		else if (BytesPerPixel == 1) {
			// 8 bit version
			unsigned char* memoryWalker = BackBuffer;
			for (int height = 0; height < BufferHeight; ++height) {
				for (int width = 0; width < BufferWidth; ++width) {

					*memoryWalker++ = rand() % 256;
				}
			}

			drawPic8(10, 10, discWidth, discHeight, discData, BackBuffer);
			drawPic8(100, 100, pauseWidth, pauseHeight, pauseData, BackBuffer);
			//drawRect8(10, 10, 300, 150, 1, BackBuffer);
		}

		// Get device context
		// Abstraction to get window state to draw stuff to it
		HDC dc = GetDC(MainWindow);

		StretchDIBits(dc, 
			0, 0, WindowWidth, WindowHeight, 
			0, 0, BufferWidth, BufferHeight, 
			BackBuffer, (BITMAPINFO*)&BitMapInfo, DIB_RGB_COLORS, SRCCOPY);

		DeleteObject(dc);

	}

	free(BackBuffer);
	free(discData);
	free(pauseData);


	return EXIT_SUCCESS;
};