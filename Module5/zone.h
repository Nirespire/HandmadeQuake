#pragma once

void Memory_Init(void* buffer, int bufferSize);
void* Hunk_AllocName(int requestSize, char* requestName);
void* Hunk_Alloc(int requestSize);
void Hunk_Check(void);