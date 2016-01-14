#include "quakedef.h"

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