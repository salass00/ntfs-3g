#ifndef STDIO_H
#define STDIO_H

#include_next <stdio.h>

#include <stdarg.h>

int my_vprintf(const char *fmt, va_list args);
int my_printf(const char *fmt, ...);
int my_vfprintf(FILE *s, const char *fmt, va_list args);
int my_fprintf(FILE *s, const char *fmt, ...);

#define vprintf(fmt,args)      my_vprintf(fmt, args)
#define printf(fmt,args...)    my_printf(fmt, ## args)
#define vfprintf(s,fmt,args)   my_vfprintf(s, fmt, args)
#define fprintf(s,fmt,args...) my_fprintf(s, fmt, ## args)

#ifndef __AROS__
int my_vsnprintf(char *buffer, size_t size, const char *fmt, va_list arg);
int my_snprintf(char *buffer, size_t size, const char *fmt, ...);
#define vsnprintf(buffer,size,fmt,args)   my_vsnprintf(buffer, size, fmt, args)
#define snprintf(buffer,size,fmt,args...) my_snprintf(buffer, size, fmt, ## args)
#endif

#endif

