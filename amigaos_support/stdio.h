#ifndef STDIO_H
#define STDIO_H

#include_next <stdio.h>

int my_vprintf(const char *fmt, va_list args);
int my_printf(const char *fmt, ...);
int my_vfprintf(FILE *s, const char *fmt, va_list args);
int my_fprintf(FILE *s, const char *fmt, ...);

#define vprintf  my_vprintf
#define printf   my_printf
#define vfprintf my_vfprintf
#define fprintf  my_fprintf

#ifndef __AROS__
int my_vsnprintf(char *buffer, size_t size, const char *fmt, va_list arg);
int my_snprintf(char *buffer, size_t size, const char *fmt, ...);
#define vsnprintf my_vsnprintf
#define snprintf my_snprintf
#endif

#endif

