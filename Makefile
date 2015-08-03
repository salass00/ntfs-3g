CC     := i386-aros-gcc
AR     := i386-aros-ar
RANLIB := i386-aros-ranlib
RM     := rm -f

V ?= /V

CFLAGS  := -O2 -s -Wall -Werror -Wwrite-strings -fno-builtin-printf \
	-fno-builtin-fprintf -I../filesysbox/include -I. \
	-I./include/ntfs-3g -I./src -I./libdiskio -I./amigaos_support/include \
	-DHAVE_CONFIG_H -DID_NTFS_DISK=0x4e544653 -DCHAR_BIT=8
LDFLAGS := -nostartfiles
LIBS    := 

LIBNTFS3G  := libntfs-3g.a
LIBDISKIO  := libdiskio.a
LIBSUPPORT := libamigaos_support.a

TARGET  := ntfs3g-handler
VERSION := 53

LIBNTFS3G_OBJS := \
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
	libntfs-3g/amiga_io.o

LIBDISKIO_OBJS := \
	libdiskio/setup.o \
	libdiskio/cleanup.o \
	libdiskio/update.o \
	libdiskio/query.o \
	libdiskio/readbytes.o \
	libdiskio/writebytes.o \
	libdiskio/flushiocache.o \
	libdiskio/diskio.o \
	libdiskio/blockcache.o \
	libdiskio/splaytree.o \
	libdiskio/mergesort.o

LIBSUPPORT_OBJS := \
	amigaos_support/debugf.o \
	amigaos_support/malloc.o \
	amigaos_support/printf.o \
	amigaos_support/syslog.o

STARTOBJ := \
	src/ntfs3g-startup_amigaos.o

OBJS := \
	src/ntfs-3g.o \
	src/ntfs-3g_common.o \
	src/ntfs-3g_amigaos.o \
	ntfsprogs/utils.o \
	ntfsprogs/sd.o \
	ntfsprogs/boot.o \
	ntfsprogs/attrdef.o
#	src/ctype.o \

.PHONY: all
all: $(TARGET)

$(LIBNTFS3G): $(LIBNTFS3G_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@

$(LIBDISKIO): $(LIBDISKIO_OBJS)
	$(AR) -crv $@ $^
	$(RANLIB) $@

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
	bumprev $(VERSION) $(TARGET)

