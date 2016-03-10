#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

#define MAX_FILES_IN_PACK 2048

typedef struct {
	char Magic[4];
	int DirectoryOffset;
	int DirectoryLength;
} dpackheader_t;

typedef struct {
	char FileName[56];
	int FilePosition;
	int FileLength;
} dpackfile_t;

typedef struct {
	char FileName[64];
	int FilePosition;
	int FileLength;
} packfile_t;

typedef struct {
	char PackName[128];
	int PackHandle;
	int NumberOfFiles;
	packfile_t* PackFiles;
} pack_t;

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

pack_t* COM_LoadPackFile(char *pakFile) {
	int pakSize;
	int pakHandle = Sys_FileOpenRead(pakFile, &pakSize);

	if (pakHandle == -1) {
		int a = 5; a;
	}

	dpackheader_t pakHeader;
	Sys_FileRead(pakHandle, &pakHeader, sizeof(pakHeader));

	// Check magic number
	if (pakHeader.Magic[0] != 'P' || pakHeader.Magic[1] != 'A' || pakHeader.Magic[2] != 'C' || pakHeader.Magic[3] != 'K') {
		int a = 5; a;
	}


	// Get the number of files that exist in the PAK file
	int numPackFiles = pakHeader.DirectoryLength / sizeof(dpackfile_t);

	// Hold the opened file in memory
	packfile_t* filesInMemory = malloc(numPackFiles * sizeof(packfile_t));

	Sys_FileSeek(pakHandle, pakHeader.DirectoryOffset);

	dpackfile_t tempPackFile[MAX_FILES_IN_PACK];
	Sys_FileRead(pakHandle, (void*)tempPackFile, pakHeader.DirectoryLength);

	for (int i = 0; i < numPackFiles; ++i) {
		strcpy_s(filesInMemory[i].FileName, 56, tempPackFile[i].FileName);
		filesInMemory[i].FileLength = tempPackFile[i].FileLength;
		filesInMemory[i].FilePosition = tempPackFile[i].FilePosition;
	}

	// Pack up files and info into a pack object
	pack_t* Pak0 = (void*)malloc(sizeof(pack_t));
	strcpy_s(Pak0->PackName, 64, pakFile);
	Pak0->PackHandle = pakHandle;
	Pak0->NumberOfFiles = numPackFiles;
	Pak0->PackFiles = filesInMemory;

	return Pak0;
}

int main() {

	
	pack_t* Pak0 = COM_LoadPackFile("../../id1/PAK0.PAK");
	pack_t* Pak1 = COM_LoadPackFile("../../id1/PAK1.PAK");

	int a = 5; a;

	return 0;
}