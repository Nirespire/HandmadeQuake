#include <stdio.h>
#include <stdarg.h>

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

	char* res = va("%d", 5.0f, 10);

	FILE* f;
	fopen_s(&f, "../Source.c", "rb");
	int pos;
	int end;

	// Pos is set to the current position of the file pointer
	// Gets set to 0 since it's at the beginning
	pos = ftell(f);

	// Move an offset of 0 bytes to end of file
	fseek(f, 0, SEEK_END);
	
	// end gets set to size of file in bytes
	end = ftell(f);

	// Move pointer back to beginning
	fseek(f, pos, SEEK_SET);

	char fileData[1024];
	fread_s(fileData, 1024, 1, end, f);

	fclose(f);

	return 0;
}