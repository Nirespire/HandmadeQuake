#pragma once

typedef struct {
	uint8* BackBuffer;
	uint32* ColorPtr;
	int32 BufferHeight;
	int32 BufferWidth;
	// 4 means 32 bit RGB mode
	// 1 means 8 bit palletized mode
	int32 BytesPerPixel;
} viddef_t;

// Global stuct to hold backbuffer and other video information
extern viddef_t Vid;

void VID_InitWindowedMode(void);
void VID_SetMode(int modeIndex);
void VID_Init(void);
void VID_Update(void);
void VID_Shutdown(void);