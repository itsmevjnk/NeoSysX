#ifndef __LIBC_STDIO_H
#define __LIBC_STDIO_H

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* from mpaland's printf implementation */
int printf(const char* format, ...) __attribute__((format(gnu_printf, 1, 2)));
int vprintf(const char* format, va_list arg) __attribute__((format(gnu_printf, 1, 0)));
int sprintf(char* s, const char* format, ...) __attribute__((format(gnu_printf, 2, 3)));
int vsprintf(char* s, const char* format, va_list arg) __attribute__((format(gnu_printf, 2, 0)));
int snprintf(char* s, size_t count, const char* format, ...) __attribute__((format(gnu_printf, 3, 4)));
int vsnprintf(char* s, size_t count, const char* format, va_list arg) __attribute__((format(gnu_printf, 3, 0)));
int fctprintf(void (*out)(char c, void* extra_arg), void* extra_arg, const char* format, ...) __attribute__((format(gnu_printf, 3, 4)));
int vfctprintf(void (*out)(char c, void* extra_arg), void* extra_arg, const char* format, va_list arg) __attribute__((format(gnu_printf, 3, 0)));

/* functions implemented by libc/libk */
int putchar(int ch);
int puts(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIBC_STRING_H */