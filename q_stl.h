#pragma once

/**
	Copies src string to dest
*/
void Q_strcpy(uint8 *dest, uint8 *src);

/**
	Copies count number of elements from src to dest.
	Pads extra space in dest with zeroes.
	Output is always count long.
*/
void Q_strncpy(uint8 *dest, uint8 *src, int32 count);


/**
	Returns the length of string str
*/
int32 Q_strlen(uint8 *str);


/**
	String compare function
	s1 == s2 -> 0
	s1 > s2 -> 1
	s1 < s2 -> -1
*/
int32 Q_strcmp(uint8 *s1, uint8 *s2);

/**
	Ascii to integer
	Can handle decimal or hex
*/
int32 Q_atoi(uint8 *str);