#pragma once

void sys_shutdown(void);
void sys_sendKeyEvents(void);

// FILE LIBRARY
static int findHandle(void);
static int fileLength(FILE *f);
int Sys_FileOpenRead(char *path, int *size);
int Sys_FileOpenWrite(char *path);
void Sys_FileClose(int handle);
void Sys_FileSeek(int handle, int pos);
int Sys_FileRead(int handle, void* dest, int count);
int Sys_FileWrite(int handle, void* source, int count);