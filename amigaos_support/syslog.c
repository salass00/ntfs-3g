#include <syslog.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <debugf.h>

#define LOGIDENTMAX 256

static char ms_logident[LOGIDENTMAX];

int daemon(int nochdir, int noclose) {
	return 0;
}

void openlog(const char *ident, int option, int facility) {
	strlcpy(ms_logident, ident, LOGIDENTMAX);
}

void closelog(void) {
	/* No-op */
}

void vsyslog(int pri, const char *fmt, void *args) {
	char buffer[256];
	size_t len;
	len = vsnprintf(buffer, sizeof(buffer), fmt, args);
	if (len >= sizeof(buffer))
		len = sizeof(buffer) - 1;
	if (len > 0 && buffer[len - 1] == '\n')
		debugf("%s: %s", ms_logident, buffer);
	else
		debugf("%s: %s\n", ms_logident, buffer);
}

void syslog(int pri, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsyslog(pri, fmt, ap);
	va_end(ap);
}

