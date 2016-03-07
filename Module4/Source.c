#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "file.h"

char* va(char* format, ...) {
	// Used to walk through variable arg list
	va_list va;

	// init global array to write into
	static char b[1024];

	// Move va to the first arg in the var arg list
	va_start(va, format);

	vsprintf_s(b, 1024, format, va);

	// Clean up memory
	va_end(va);

	return b;

}

int main() {

	int size;

	int fileHandle = Sys_FileOpenRead("../../Source.c", &size);

	if (fileHandle == -1) {
		int a = 5; a;
	}

	void* buffer = malloc(size);

	Sys_FileRead(fileHandle, buffer, size);

	int writeFile = Sys_FileOpenWrite("../../Source.out");

	if (writeFile == -1) {
		int a = 5; a;
	}

	Sys_FileWrite(writeFile, buffer, size);

	Sys_FileClose(fileHandle);
	Sys_FileClose(writeFile);

	free(buffer);
	buffer = NULL;

	// char* colorData = LoadFile("palette.lmp");

	return 0;
}