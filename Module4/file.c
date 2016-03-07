#include <stdio.h>
#include "file.h"

#define MAX_HANDLES 10
static FILE* FileHandles[MAX_HANDLES] = { 0 };

static int findHandle(void) {
	for(int i = 0; i < MAX_HANDLES; ++i) {
		if (FileHandles[i] == 0) {
			return i;
		}
	}

	return -1;
}

static int fileLength(FILE *f) {
	int pos;
	int end;

	pos = ftell(f);

	fseek(f, 0, SEEK_END);

	end = ftell(f);

	fseek(f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead(char *path, int *size) {
	int handleIdx = findHandle();

	FILE* fileHandle = FileHandles[handleIdx];

	fopen_s(&fileHandle, path, "rb");

	if (fileHandle == 0) {
		if (size != 0) {
			*size = -1;
		}
		return -1;
	}

	FileHandles[handleIdx] = fileHandle;

	if (size != 0) {
		*size = fileLength(fileHandle);
	}

	return handleIdx;
}

int Sys_FileOpenWrite(char *path) {
	int handleIdx = findHandle();

	FILE* fileHandle = FileHandles[handleIdx];

	fopen_s(&fileHandle, path, "wb");


	if (fileHandle == 0) {
		handleIdx = -1;
	}

	FileHandles[handleIdx] = fileHandle;

	return handleIdx;
}

void Sys_FileClose(int handle) {
	if (handle < 0 || handle > MAX_HANDLES) {
		return;
	}

	if (FileHandles[handle] != NULL) {
		fclose(FileHandles[handle]);
		FileHandles[handle] = NULL;
	}
}

void Sys_FileSeek(int handle, int pos) {
	if (handle < 0 || handle > MAX_HANDLES) {
		return;
	}

	if (FileHandles[handle] != NULL) {
		fseek(FileHandles[handle], pos, SEEK_SET);
	}
}

int Sys_FileRead(int handle, void* dest, int count) {
	if (handle < 0 || handle > MAX_HANDLES || dest == 0) {
		return 0;
	}

	// This cast assumes not more than 32GB read at once
	int bytesRead = (int)fread_s(dest, count, 1, count, FileHandles[handle]);

	return bytesRead;
}

int Sys_FileWrite(int handle, void* source, int count) {
	if (handle < 0 || handle > MAX_HANDLES || source == 0) {
		return 0;
	}

	int bytesWritten = (int)fwrite(source, 1, count, FileHandles[handle]);

	return bytesWritten;
}