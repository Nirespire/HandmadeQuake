#pragma once

#include <windows.h>
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

extern HINSTANCE globalInstance;