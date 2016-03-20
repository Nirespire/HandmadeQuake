#include "quakedef.h"

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

/***************
*	COMMAND LINE PARSING *
****************/

int32 com_argc = 1;
const uint8 *com_argv[MAX_NUM_ARGVS + 1];

void COM_ParseCmdLine(uint8 *lpCmdLine) {

	// Initialize first element in arg array to 0
	com_argv[0] = "";

	while (*lpCmdLine && (com_argc < MAX_NUM_ARGVS)) {
		// Skip any invalid characters
		while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126))) {
			++lpCmdLine;
		}

		// At the start of each word, save start pointer in com_argv and clear out lpCmdLine
		if (*lpCmdLine) {
			com_argv[com_argc] = lpCmdLine;
			++com_argc;

			while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126))) {
				lpCmdLine++;
			}

			if (*lpCmdLine) {
				*lpCmdLine = 0;
				++lpCmdLine;
			}
		}
	}
}

int32 COM_CheckParm(uint8 *parm) {

	for (int32 i = 1; i < com_argc; i++) {
		if (!Q_strcmp(parm, com_argv[i])) {
			return i;
		}
	}

	return 0;
}


/***************
*	END LINE PARSING *
****************/



/***************
*	FILE MANAGEMENT *
****************/


// typedefs
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
#pragma pack(pop)

typedef struct searchpath_s {
	pack_t* Pack;
	struct searchpath_s* Next;
} searchpath_t;

// data structures
#define MAX_FILES_IN_PACK 2048
searchpath_t* COM_Searchpaths = NULL;

// functions
static pack_t* COM_LoadPackFile(char *pakFile) {
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
		//strcpy_s(filesInMemory[i].FileName, 56, tempPackFile[i].FileName);
		Q_strcpy(filesInMemory[i].FileName, tempPackFile[i].FileName);
		filesInMemory[i].FileLength = tempPackFile[i].FileLength;
		filesInMemory[i].FilePosition = tempPackFile[i].FilePosition;
	}

	// Pack up files and info into a pack object
	pack_t* newPak = (void*)malloc(sizeof(pack_t));
	Q_strcpy(newPak->PackName, pakFile);
	newPak->PackHandle = pakHandle;
	newPak->NumberOfFiles = numPackFiles;
	newPak->PackFiles = filesInMemory;

	return newPak;
}



// Load all PAK files in specific directory into a linked list
static void COM_AddGameDirectory(const char *directory) {

	char buffer[128];
	pack_t* pakPointer;

	for (int i = 0; ; ++i) {
		sprintf_s(buffer, 128, "%s\\PAK%d.PAK", directory, i);
		pakPointer = COM_LoadPackFile(buffer);

		if (!pakPointer) {
			break;
		}

		searchpath_t* newPath = (searchpath_t*)malloc(sizeof(searchpath_t));
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
			if (!Q_strcmp(filename, currentPak->PackFiles[i].FileName)) {
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

void COM_FileInit(void) {
	COM_AddGameDirectory("..\\..\\id1");
}

void COM_FileShutdown(void) {
	while (COM_Searchpaths) {
		searchpath_t* toDelete = COM_Searchpaths;

		COM_Searchpaths = COM_Searchpaths->Next;

		free(toDelete->Pack->PackFiles);
		free(toDelete->Pack);
		free(toDelete);
	}
}


/***************
*	END FILE MANAGEMENT *
****************/