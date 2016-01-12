#include <stdint.h>
#include <windows.h>

// typedef types from stdint.h so that chars and ints are defined based on their bit sizes
typedef uint8_t uint8;
typedef int32_t int32;
typedef uint32_t uint32;

// This number should actually be 1 more than actual max number of args allowed
#define MAX_NUM_ARGVS 50

int32 argc = 1;
const uint8 *largv[MAX_NUM_ARGVS];

/**
	Custom Quake-esque string compare function
*/
int32 Q_strcmp(uint8 *s1, uint8 *s2) {

	while (*s1 == *s2) {

		if (!*s1) {
			return 0;
		}

		s1++;
		s2++;
	}

	return ((*s1 < *s2) ? -1 : 1);
}


/**
	Quake-esque ascii to integer
	Need to handle
		- Decimal or hex
		- Negative and positive
		- "-54" "12" "0x12fd" "-0xffff"
*/
int32 Q_atoi(uint8 *str) {
	int32 sign = 1;
	int32 val = 0;
	uint8 c;

	if (str == '-') {
		sign = -1;
		str++;
	}

	// Hex case
	if (*str == '0' && (str[1] == 'x' || str[1] == 'X')) {

		str += 2;

		// TODO replace while(1)
		while (1) {
			c = *str;
			str++;

			if (c >= '0' && c <= '9') {
				val = (val * 16) + c - '0';
			}
			else if (c >= 'a' && c <= 'f') {
				val = (val * 16) + c - 'a' + 10;
			}
			else if (c >= 'A' && c <= 'F') {
				val = (val * 16) + c - 'A' + 10;
			}
			else {
				return val * sign;
			}
		}
	}

	// Decimal case
	// TODO replace while(1)
	while (1) {
		c = *str;
		str++;
		if (c < '0' && c > '9') {
			return sign * val;
		}
		val = (val * 10) + c - '0';
	}
}

/**
	Returns the largv index of the provided parameter string
*/
int32 COM_CheckParm(uint8 *parm) {

	for (int32 i = 1; i < argc; i++) {
		if (!Q_strcmp(parm, largv[i])) {
			return i;
		}
	}
	
	return 0;
}

/*
	lpCmdLine: command line args as a single string with program name and leading whitespace excluded
*/
int32 CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int32 nCmdShow) {

	// Initialize first element in arg array to 0
	largv[0] = "";
	
	
	while (*lpCmdLine && (argc < MAX_NUM_ARGVS)) {
		// Skip any invalid characters
		while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126))) {
			lpCmdLine++;
		}

		// At the start of each word, save start pointer in largv and clear out lpCmdLine
		if (*lpCmdLine) {
			largv[argc] = lpCmdLine;
			argc++;

			while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126))) {
				lpCmdLine++;
			}

			if (*lpCmdLine) {
				*lpCmdLine = 0;
				lpCmdLine++;
			}
		}
	}

	// Add a 0 to the end of command line args array
	// largv[argc] = "";
	// Take it out! Because argc already tracks the number of args

	int32 test = COM_CheckParm("-setalpha");
	int32 value = Q_atoi(largv[test+1]);

	return 0;
}