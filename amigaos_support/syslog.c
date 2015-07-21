#include <syslog.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <proto/arossupport.h>

#define LOGIDENTMAX 256

static char ms_logident[LOGIDENTMAX];

int daemon(int nochdir, int noclose) {
	return 0;
}

void openlog(const char *ident, int option, int facility) {
	strlcpy(ms_logident, ident, LOGIDENTMAX);
}

void closelog(void) {
}

static char lastchar(const char *str) {
	int i = strlen(str) - 1;
	return i >= 0 ? str[i] : '\0';
}

void vsyslog(int pri, const char *fmt, void *args) {
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	kprintf("%s: %s%s", ms_logident, buffer, lastchar(buffer) == '\n' ? "" : "\n");
}

void syslog(int pri, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsyslog(pri, fmt, ap);
	va_end(ap);
}

