#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

void* memset(void* dest, int fll, size_t nbytes);
void* memmove(void* dest, const void* src, size_t nbytes);
void* memcpy(void* dest, const void* src, size_t nbytes);
void* memccpy(void* dest, const void* src, int stop, size_t nbytes);
int memcmp(const void* src1, const void* src2, size_t nbytes);

size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
int stoi(void* strv, int len);
void htos(uint32_t hex, char str[]);
void dtos(uint32_t dec, char str[]);
int atoi (const char * str);
char * itoa( int value, char * str, int base );

#endif