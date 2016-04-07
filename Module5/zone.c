#define _CRT_SECURE_NO_WARNINGS
#include "zone.h"
#include <string.h>
// Used to identify the start of a valid piece of memory
#define HUNK_SENTINAL 0x1df001ed

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
	header->memorySentinal = HUNK_SENTINAL;
	strcpy(header->name, requestName);

	return (void*)(header + 1);
}