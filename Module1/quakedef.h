/**
	quakedef.h
	Single include for all c files that will inclusively include all other .h files
*/

#pragma once
#include <stdint.h>
#include<stdio.h>

// typedef types from stdint.h so that chars and ints are defined based on their bit sizes
typedef uint8_t uint8;
typedef int32_t int32;
typedef uint32_t uint32;

#include "common.h"
#include "q_stl.h"
#include "host.h"