#ifndef _SYSLOG_H
#define _SYSLOG_H

/* option flags */
#define LOG_PID 0x01 /* log the pid with each message */

/* facility codes */
#define LOG_USER   (1<<3) /* random user-level messages */
#define LOG_DAEMON (3<<3) /* system daemons */

/* priorities */
#define LOG_EMERG   0 /* system is unusable */
#define LOG_ALERT   1 /* action must be taken immediately */
#define LOG_CRIT    2 /* critical conditions */
#define LOG_ERR     3 /* error conditions */
#define LOG_WARNING 4 /* warning conditions */
#define LOG_NOTICE  5 /* normal but significant condition */
#define LOG_INFO    6 /* informational */
#define LOG_DEBUG   7 /* debug-level messages */

int daemon(int nochdir, int noclose);
void openlog(const char *ident, int option, int facility);
void closelog(void);
void syslog(int pri, const char *fmt, ...);
void vsyslog(int pri, const char *fmt, void *);

#endif

