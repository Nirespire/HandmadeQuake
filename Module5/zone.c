#define _CRT_SECURE_NO_WARNINGS
#include "zone.h"
#include <string.h>
#include <assert.h>
// Used to identify the start of a valid piece of memory
#define HUNK_SENTINEL 0x1df001ed

typedef struct {
	int memorySentinal;
	int memorySize;
	char name[8];
} hunk_t;

// Hunk stuff
char* HunkBase;
int HunkSize;
int HunkLowOffset;
int HunkHighOffset;

// Zone stuff
#define ZONESIZE 0xc000

void Memory_Init(void* buffer, int bufferSize) {
	HunkBase = buffer;
	HunkSize = bufferSize;

	HunkLowOffset = 0;
	HunkHighOffset = 0;

	// Allocate 48k on Hunk for Zone space
	//void* zoneSpace = Hunk_AllocName(ZONESIZE, "zone");
}

void Hunk_Check(void) {
	for (hunk_t* header = (hunk_t*)HunkBase; (char*)header != (HunkBase + HunkLowOffset);) {
		
		if (header->memorySentinal != HUNK_SENTINEL) {
			assert(0);
		}

		if (header->memorySize < 16 || (char*)header - HunkBase + header->memorySize > HunkSize) {
			assert(0);
		}

		header = (hunk_t*)((char*)header + header->memorySize);
	}
}

void* Hunk_Alloc(int requestSize) {
	return Hunk_AllocName(requestSize, "unknown");
}

void* Hunk_AllocName(int requestSize, char* requestName) {

	// Round up to closest multiple of 16
	int totalSize = sizeof(hunk_t) + ((requestSize+15)& ~15);

	hunk_t* header;

	header = (hunk_t*)(HunkBase + HunkLowOffset);
	HunkLowOffset += totalSize;

	memset(header, 0, totalSize);

	header->memorySize = totalSize;
	header->memorySentinal = HUNK_SENTINEL;
	// want to use strncpy because the name could be longet than 8 bytes
	strncpy(header->name, requestName, 8);

	return (void*)(header + 1);
}

// Reset the memory stack pointer
void Hunk_FreeToLowMark(int mark) {
	memset(HunkBase + mark, 0, HunkLowOffset - mark);
	HunkLowOffset = mark;
}

void* Hunk_HighAllocName(int requestSize, char* requestName) {
	int totalSize = sizeof(hunk_t) + ((requestSize + 15)& ~15);

	hunk_t* header;

	HunkHighOffset += totalSize;

	header = (hunk_t*)(HunkBase + HunkSize - HunkHighOffset);
	

	memset(header, 0, totalSize);

	header->memorySize = totalSize;
	header->memorySentinal = HUNK_SENTINEL;
	strncpy(header->name, requestName, 8);

	return (void*)(header + 1);
}

void Hunk_FreeToHighMark(int mark) {
	memset(HunkBase + HunkSize - HunkHighOffset, 0, HunkHighOffset - mark);
	HunkHighOffset = mark;
}