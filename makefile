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
WARNINGS = -Werror -Wall -Wwrite-strings -Wno-unused-const-variable \
           -Wno-address-of-packed-member -Wno-zero-length-bounds

CFLAGS  = -O2 -g -fomit-frame-pointer -fno-builtin-printf -fno-builtin-fprintf \
          $(INCLUDES) $(DEFINES) $(WARNINGS)
LDFLAGS = -nostartfiles
LIBS    = -ldebug

ifneq (,$(SYSROOT))
	CFLAGS  := --sysroot=$(SYSROOT) $(CFLAGS)
	LDFLAGS := --sysroot=$(SYSROOT) $(LDFLAGS)
endif

ifneq (,$(findstring -aros,$(HOST)))
	CPU = $(patsubst %-aros,%,$(HOST))
else
	CPU = 68000
endif

ifeq ($(HOST),m68k-amigaos)
	CFLAGS  := -noixemul $(CFLAGS)
	LDFLAGS := -noixemul $(LDFLAGS)
	LIBS =
endif

LIBNTFS3G  = libntfs-3g.a
LIBDISKIO  = libdiskio.a
LIBSUPPORT = libamigaos_support.a

LIBNTFS3G_SRCS = \
	libntfs-3g/bitmap.c \
	libntfs-3g/misc.c \
	libntfs-3g/bootsect.c \
	libntfs-3g/collate.c \
	libntfs-3g/attrib.c \
	libntfs-3g/cache.c \
	libntfs-3g/dir.c \
	libntfs-3g/ea.c \
	libntfs-3g/efs.c \
	libntfs-3g/index.c \
	libntfs-3g/inode.c \
	libntfs-3g/acls.c \
	libntfs-3g/unistr.c \
	libntfs-3g/runlist.c \
	libntfs-3g/mst.c \
	libntfs-3g/mft.c \
	libntfs-3g/logfile.c \
	libntfs-3g/lcnalloc.c \
	libntfs-3g/debug.c \
	libntfs-3g/device.c \
	libntfs-3g/volume.c \
	libntfs-3g/reparse.c \
	libntfs-3g/object_id.c \
	libntfs-3g/attrlist.c \
	libntfs-3g/compress.c \
	libntfs-3g/logging.c \
	libntfs-3g/security.c \
	libntfs-3g/realpath.c \
	libntfs-3g/xattrs.c \
	libntfs-3g/compat.c \
	libntfs-3g/amiga_io.c

LIBDISKIO_SRCS = \
	libdiskio/setup.c \
	libdiskio/cleanup.c \
	libdiskio/update.c \
	libdiskio/query.c \
	libdiskio/readbytes.c \
	libdiskio/writebytes.c \
	libdiskio/flushiocache.c \
	libdiskio/deviceio.c \
	libdiskio/cachedio.c \
	libdiskio/blockcache.c \
	libdiskio/memhandler.c \
	libdiskio/splay.c

LIBSUPPORT_SRCS = \
	amigaos_support/debugf.c \
	amigaos_support/gettimeofday.c \
	amigaos_support/kputstr.c \
	amigaos_support/malloc.c \
	amigaos_support/printf.c \
	amigaos_support/random.c \
	amigaos_support/snprintf.c \
	amigaos_support/strdup.c \
	amigaos_support/strlcpy.c \
	amigaos_support/syslog.c

SRCS = \
	src/ntfs3g-startup_amigaos.c \
	src/ntfs-3g.c \
	src/ntfs-3g_common.c \
	src/ntfs-3g_amigaos.c \
	ntfsprogs/utils.c \
	ntfsprogs/sd.c \
	ntfsprogs/boot.c \
	ntfsprogs/attrdef.c

LIBNTFS3G_OBJS = $(addprefix obj/$(CPU)/,$(LIBNTFS3G_SRCS:.c=.o))
LIBDISKIO_OBJS = $(addprefix obj/$(CPU)/,$(LIBDISKIO_SRCS:.c=.o))
LIBSUPPORT_OBJS = $(addprefix obj/$(CPU)/,$(LIBSUPPORT_SRCS:.c=.o))
OBJS = $(addprefix obj/$(CPU)/,$(SRCS:.c=.o))

DEPS = $(LIBNTFS3G_OBJS:.o=.d) \
       $(LIBDISKIO_OBJS:.o=.d) \
       $(LIBSUPPORT_OBJS:.o=.d) \
       $(OBJS:.o=.d)

.PHONY: all
all: bin/$(TARGET).$(CPU) bin/$(TARGET).$(CPU).debug

-include $(DEPS)

obj/$(CPU)/libntfs-3g/%.o: libntfs-3g/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(CFLAGS) $<
	$(CC) $(CFLAGS) -c -o $@ $<

obj/$(CPU)/libdiskio/%.o: libdiskio/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(CFLAGS) $<
	$(CC) $(CFLAGS) -c -o $@ $<

obj/$(CPU)/amigaos_support/%.o: amigaos_support/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(CFLAGS) $<
	$(CC) $(CFLAGS) -c -o $@ $<

obj/$(CPU)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(CFLAGS) $<
	$(CC) $(CFLAGS) -c -o $@ $<

obj/$(CPU)/ntfsprogs/%.o: ntfsprogs/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(CFLAGS) $<
	$(CC) $(CFLAGS) -c -o $@ $<

bin/$(LIBNTFS3G).$(CPU): $(LIBNTFS3G_OBJS)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBDISKIO).$(CPU): $(LIBDISKIO_OBJS)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

obj/$(CPU)/amigaos_support/malloc.o: CFLAGS += -fno-builtin

bin/$(LIBSUPPORT).$(CPU): $(LIBSUPPORT_OBJS)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(TARGET).$(CPU).debug: $(OBJS) bin/$(LIBNTFS3G).$(CPU) bin/$(LIBDISKIO).$(CPU) bin/$(LIBSUPPORT).$(CPU)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/$(TARGET).$(CPU): $(OBJS) bin/$(LIBNTFS3G).$(CPU) bin/$(LIBDISKIO).$(CPU) bin/$(LIBSUPPORT).$(CPU)
	@mkdir -p $(dir $@)
	$(CC) -s $(LDFLAGS) -o $@ $^ $(LIBS)


.PHONY: clean
clean:
	rm -rf bin obj

.PHONY: revision
revision:
	bumprev $(VERSION) NTFileSystem3G
	bumprev $(VERSION) ntfs3g-handler

