#include "quakedef.h"


void drawPic32(int x, int y, int width, int height, unsigned char* source) {

	uint8* dest = Vid.BackBuffer;

	// move to first pixel
	dest += (Vid.BufferWidth * Vid.BytesPerPixel * y) + (x * Vid.BytesPerPixel);

	unsigned int* bufferWalker = (unsigned int*)dest;

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {

			// Pull out the RGB values from the pallet and draw them directly to the object
			int* colorValue = &Vid.ColorPtr[*source];

			// Add transparency
			if (*source != 0xff) {
				*bufferWalker = *colorValue;
			}
			++bufferWalker;
			++source;
		}

		dest += Vid.BufferWidth * Vid.BytesPerPixel;
		bufferWalker = (unsigned int*)dest;
	}
}