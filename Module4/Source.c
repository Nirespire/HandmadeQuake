#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

#define MAX_FILES_IN_PACK 2048

#pragma pack(push,1)
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

typedef struct searchpath_s {
	pack_t* Pack;
	struct searchpath_s* Next;
} searchpath_t;
#pragma pack(pop)

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
		return NULL;
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

searchpath_t* COM_Searchpaths = NULL;

// Load all PAK files in specific directory into a linked list
void COM_AddGameDirectory(const char *directory) {
	
	char buffer[128];
	pack_t* pakPointer;

	for (int i = 0; ; ++i) {
		sprintf_s(buffer, 128, "%s\\PAK%d.PAK", directory, i);
		pakPointer = COM_LoadPackFile(buffer);

		if (!pakPointer) {
			break;
		}

		searchpath_t* newPath = (searchpath_t*) malloc(sizeof(searchpath_t));
		newPath->Pack = pakPointer;
		newPath->Next = COM_Searchpaths;
		COM_Searchpaths = newPath;

	}
}

char* COM_FindFile(const char* filename, int* fileLength) {

	if (!filename) {
		return NULL;
	}

	searchpath_t* jogger;

	for (jogger = COM_Searchpaths; jogger != NULL; jogger = jogger->Next) {
		pack_t* currentPak = jogger->Pack;

		for (int i = 0; i < currentPak->NumberOfFiles; ++i) {
			if (!strcmp(filename, currentPak->PackFiles[i].FileName)) {
				Sys_FileSeek(currentPak->PackHandle, currentPak->PackFiles[i].FilePosition);
				
				char* fileData = malloc(currentPak->PackFiles[i].FileLength);
				Sys_FileRead(currentPak->PackHandle, fileData, currentPak->PackFiles[i].FileLength);

				if (fileLength) {
					*fileLength = currentPak->PackFiles[i].FileLength;
				}
				return (char*)fileData;
			}
		}
	}


	return NULL;
}

int main() {

	COM_AddGameDirectory("..\\..\\id1");

	int length = 0;
	char* paletteBits = COM_FindFile("gfx/palette.lmp", &length);

	int a = 5; a;

	return 0;
}