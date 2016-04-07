#include "quakedef.h"

typedef struct {
	int32	filePosition;
	int32	diskSize;
	int32	size;
	int8	fileType;
	int8	compressionType;
	int16	padding;
	int8	name[16];
} lumpinfo_t;


typedef struct {
	int8	magic[4];
	int32	numberOfLumps;
	int32	directoryOffset;
}wadinfo_t;

static int8* WadFile;
static int32 LumpCount;
static lumpinfo_t* Lumps;

static void W_cleanupName(int8* in, int8* out) {
	int i = 0;
	
	for (i = 0; i < 16; ++i) {
		char c = in[i];

		if (c == 0) {
			break;
		}

		if (c >= 'A' && c <= 'Z') {
			c |= 32;
		}

		out[i] = c;
	}

	for (; i < 16; ++i) {
		out[i] = 0;
	}
}

void W_loadWadFile(int8* filename) {

	int32 fileOffsetCount = 0;

	int length = 0;
	WadFile = COM_FindFile(filename, &length);

	if (!WadFile) {
		assert(0);
	}

	wadinfo_t* wadHeader = (wadinfo_t*)WadFile;

	if (wadHeader->magic[0] != 'W'
		|| wadHeader->magic[1] != 'A'
		|| wadHeader->magic[2] != 'D'
		|| wadHeader->magic[3] != '2') {
		assert(0);
	}

	LumpCount = wadHeader->numberOfLumps;
	fileOffsetCount = wadHeader->directoryOffset;

	Lumps = (lumpinfo_t*)(WadFile + fileOffsetCount);

	
	lumpinfo_t* lumpWalker = Lumps;
	for (int i = 0; i < LumpCount; ++i) {
		W_cleanupName(lumpWalker->name, lumpWalker->name);
		++lumpWalker;
	}


}

static lumpinfo_t* W_getLumpInfo(int8* name) {
	int8 cleanName[16];

	W_cleanupName(name, cleanName);

	lumpinfo_t* lumpWalker = Lumps;

	for (int i = 0; i < LumpCount; ++i) {
		if (!Q_strcmp(cleanName, lumpWalker->name)) {
			return lumpWalker;
		}
		++lumpWalker;
	}

	assert(0);
	return NULL;
}

void* W_getLumpName(int8* name) {

	lumpinfo_t* lump;
	lump = W_getLumpInfo(name);
	

	return (void*) (WadFile + lump->filePosition);
}