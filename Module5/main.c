
#include <stdlib.h>

#include "zone.h"

#define ALLOCSIZE 16*1024*1024

int main() {

	void* memoryBlock = malloc(ALLOCSIZE);

	Memory_Init(memoryBlock, ALLOCSIZE);

	int* temp = (int*)Hunk_AllocName(4, "temp");

	*temp = 4;

	Hunk_Check();

	return 0;
}