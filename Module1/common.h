#pragma once

// This number should actually be 1 more than actual max number of args allowed
#define MAX_NUM_ARGVS 50

extern int32 com_argc;
extern const uint8 *com_argv[MAX_NUM_ARGVS + 1];


/**
	Breaks up the command like args string
*/
void COM_ParseCmdLine(uint8 *lpCmdLine);


/**
	Returns the com_argv index of the provided parameter string
*/
int32 COM_CheckParm(uint8 *parm);