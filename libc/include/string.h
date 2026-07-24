#ifndef __LIBC_STRING_H
#define __LIBC_STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t count);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t count);
size_t strlen(const char* str);
size_t strnlen(const char* str, size_t maxlen);
int strcmp(const char* lhs, const char* rhs);
int strncmp(const char* lhs, const char* rhs, size_t count);
const char* strchr(const char* str, int ch);
const char* strrchr(const char* str, int ch);
const void* memchr(const void* ptr, int ch, size_t count);
int memcmp(const void* lhs, const void* rhs, size_t count);
void* memset(void* dest, int ch, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memmove(void* dest, const void* src, size_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBC_STRING_H */