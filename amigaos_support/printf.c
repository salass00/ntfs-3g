#include <stdio.h>
#include <stdarg.h>
#include <debugf.h>

int my_vprintf(const char *fmt, va_list args) {
	return vdebugf(fmt, args);
}

int my_printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int retval = vdebugf(fmt, ap);
	va_end(ap);
	return retval;
}

int my_vfprintf(FILE *s, const char *fmt, va_list args) {
	return vdebugf(fmt, args);
}

int my_fprintf(FILE *s, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int retval = vdebugf(fmt, ap);
	va_end(ap);
	return retval;
}

