HOST ?= i386-aros

CC     = $(HOST)-gcc
AR     = $(HOST)-ar
RANLIB = $(HOST)-ranlib

ifeq ($(HOST),m68k-amigaos)
	TARGET = NTFileSystem3G
else
	TARGET = ntfs3g-handler
endif
VERSION = 53

INCLUDES = -I. -I./include/ntfs-3g -I./src -I./libdiskio -I./amigaos_support/include
DEFINES  = -DHAVE_CONFIG_H -DID_NTFS_DISK=0x4e544653 -DCHAR_BIT=8 #-DDEBUG
WARNINGS = -Werror -Wall -Wwrite-strings -Wno-unused-const-variable

CFLAGS  = -O2 -g -fomit-frame-pointer -fno-builtin-printf -fno-builtin-fprintf \
          $(INCLUDES) $(DEFINES) $(WARNINGS)
LDFLAGS = -s -nostartfiles
LIBS    = -ldebug

ifneq (,$(SYSROOT))
	CFLAGS  := --sysroot=$(SYSROOT) $(CFLAGS)
	LDFLAGS := --sysroot=$(SYSROOT) $(LDFLAGS)
endif

ifeq ($(HOST),m68k-amigaos)
	CFLAGS  := -noixemul $(CFLAGS)
	LDFLAGS := -noixemul $(LDFLAGS)
	LIBS =
endif

LIBNTFS3G  = libntfs-3g.a
LIBDISKIO  = libdiskio.a
LIBSUPPORT = libamigaos_support.a

LIBNTFS3G_OBJS = \
	libntfs-3g/bitmap.o \
	libntfs-3g/misc.o \
	libntfs-3g/bootsect.o \
	libntfs-3g/collate.o \
	libntfs-3g/attrib.o \
	libntfs-3g/cache.o \
	libntfs-3g/dir.o \
	libntfs-3g/ea.o \
	libntfs-3g/efs.o \
	libntfs-3g/index.o \
	libntfs-3g/inode.o \
	libntfs-3g/acls.o \
	libntfs-3g/unistr.o \
	libntfs-3g/runlist.o \
	libntfs-3g/mst.o \
	libntfs-3g/mft.o \
	libntfs-3g/logfile.o \
	libntfs-3g/lcnalloc.o \
	libntfs-3g/debug.o \
	libntfs-3g/device.o \
	libntfs-3g/volume.o \
	libntfs-3g/reparse.o \
	libntfs-3g/object_id.o \
	libntfs-3g/attrlist.o \
	libntfs-3g/compress.o \
	libntfs-3g/logging.o \
	libntfs-3g/security.o \
	libntfs-3g/realpath.o \
	libntfs-3g/xattrs.o \
	libntfs-3g/compat.o \
	libntfs-3g/amiga_io.o

LIBDISKIO_OBJS = \
	libdiskio/setup.o \
	libdiskio/cleanup.o \
	libdiskio/update.o \
	libdiskio/query.o \
	libdiskio/readbytes.o \
	libdiskio/writebytes.o \
	libdiskio/flushiocache.o \
	libdiskio/deviceio.o \
	libdiskio/cachedio.o \
	libdiskio/blockcache.o \
	libdiskio/memhandler.o \
	libdiskio/splay.o

LIBSUPPORT_OBJS = \
	amigaos_support/debugf.o \
	amigaos_support/gettimeofday.o \
	amigaos_support/kputstr.o \
	amigaos_support/malloc.o \
	amigaos_support/printf.o \
	amigaos_support/random.o \
	amigaos_support/snprintf.o \
	amigaos_support/strdup.o \
	amigaos_support/strlcpy.o \
	amigaos_support/syslog.o

STARTOBJ = \
	src/ntfs3g-startup_amigaos.o

OBJS = \
	src/ntfs-3g.o \
	src/ntfs-3g_common.o \
	src/ntfs-3g_amigaos.o \
	ntfsprogs/utils.o \
	ntfsprogs/sd.o \
	ntfsprogs/boot.o \
	ntfsprogs/attrdef.o

.PHONY: all
all: $(TARGET)

$(LIBNTFS3G): $(LIBNTFS3G_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@

$(LIBDISKIO): $(LIBDISKIO_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@

amigaos_support/malloc.o: CFLAGS += -fno-builtin

$(LIBSUPPORT): $(LIBSUPPORT_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@

$(STARTOBJ): $(TARGET)_rev.h

$(TARGET): $(STARTOBJ) $(OBJS) $(LIBNTFS3G) $(LIBDISKIO) $(LIBSUPPORT)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(LIBNTFS3G) $(LIBDISKIO) $(LIBSUPPORT) */*.o

.PHONY: dist-clean
dist-clean:
	$(RM) $(LIBNTFS3G) $(LIBDISKIO) $(LIBSUPPORT) */*.o

.PHONY: revision
revision:
	bumprev $(VERSION) NTFileSystem3G
	bumprev $(VERSION) ntfs3g-handler

