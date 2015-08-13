#ifndef _SYS_MKDEV_H
#define _SYS_MKDEV_H

#include <sys/types.h>

#ifdef __AROS__
dev_t makedev(int maj, int min);
unsigned int major(dev_t dev);
unsigned int minor(dev_t dev);

#define makedev(maj,min) (((dev_t)(maj) & 0xffff) << 16)|((dev_t)(min) & 0xffff)
#define major(dev) (((dev) >> 16) & 0xffff)
#define minor(dev) ((dev) & 0xffff)
#endif

#endif

