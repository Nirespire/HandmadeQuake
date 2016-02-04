#pragma once

/**
	Copies src string to dest
*/
void Q_strcpy(char *dest, const char *src);

/**
	Copies count number of elements from src to dest.
	Pads extra space in dest with zeroes.
	Output is always count long.
*/
void Q_strncpy(char *dest, const char *src, int32 count);


/**
	Returns the length of string str
*/
int32 Q_strlen(const char *str);


/**
	String compare function
	s1 == s2 -> 0
	s1 > s2 -> 1
	s1 < s2 -> -1
*/
int32 Q_strcmp(const char *s1, const char *s2);

/**
	Ascii to integer
	Can handle decimal or hex
*/
int32 Q_atoi(const char *str);