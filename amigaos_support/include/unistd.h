#ifndef _UNISTD_H_

#include_next <unistd.h>

#define getuid() (0)
#define getgid() (0)
#define geteuid() (0)
#define getegid() (0)

#endif

