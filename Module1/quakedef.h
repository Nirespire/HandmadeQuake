/**
	quakedef.h
	Single include for all c files that will inclusively include all other .h files
*/

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
//#include <string.h>
#include <assert.h>

// typedef types from stdint.h so that chars and ints are defined based on their bit sizes
typedef int8_t		int8;
typedef uint8_t		uint8;
typedef int16_t		int16;
typedef uint16_t	uint16;
typedef int32_t		int32;
typedef uint32_t	uint32;

#include "common.h"
#include "q_stl.h"
#include "host.h"
#include "sys.h"
#include "vid.h"
#include "draw.h"
#include "wad.h"