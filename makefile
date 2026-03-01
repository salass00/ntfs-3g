CC     = m68k-amigaos-gcc
STRIP  = m68k-amigaos-strip
AR     = m68k-amigaos-ar
RANLIB = m68k-amigaos-ranlib

TARGET  = NTFileSystem3G
VERSION = 53

INCLUDES = -I. -I./include/ntfs-3g -I./include/amigaos3 -I./src -I./libdiskio -I./amigaos_support/include
DEFINES  = -DHAVE_CONFIG_H -DID_NTFS_DISK=0x4e544653 -DCHAR_BIT=8
WARNINGS = -Werror -Wall -Wwrite-strings -Wno-unused-const-variable

CFLAGS  = -noixemul -O2 -g -fomit-frame-pointer -fno-builtin-printf -fno-builtin-fprintf \
          $(INCLUDES) $(DEFINES) $(WARNINGS)
LDFLAGS = -noixemul -g -nostartfiles
LIBS    = -lm

STRIPFLAGS = -R.comment

ARCH_000 = -mcpu=68000 -mtune=68000
ARCH_020 = -mcpu=68020 -mtune=68020-60
ARCH_060 = -mcpu=68060 -mtune=68060

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
	amigaos_support/syslog.c \
	amigaos_support/usergroup.c

SRCS = \
	src/ntfs3g-startup_amigaos.c \
	src/ntfs-3g.c \
	src/ntfs-3g_common.c \
	src/ntfs-3g_amigaos.c \
	ntfsprogs/utils.c \
	ntfsprogs/sd.c \
	ntfsprogs/boot.c \
	ntfsprogs/attrdef.c

LIBNTFS3G_OBJS_000 = $(addprefix obj/68000/,$(LIBNTFS3G_SRCS:.c=.o))
LIBDISKIO_OBJS_000 = $(addprefix obj/68000/,$(LIBDISKIO_SRCS:.c=.o))
LIBSUPPORT_OBJS_000 = $(addprefix obj/68000/,$(LIBSUPPORT_SRCS:.c=.o))
OBJS_000 = $(addprefix obj/68000/,$(SRCS:.c=.o))
DEPS_000 = $(LIBNTFS3G_OBJS_000:.o=.d) \
           $(LIBDISKIO_OBJS_000:.o=.d) \
           $(LIBSUPPORT_OBJS_000:.o=.d) \
           $(OBJS_000:.o=.d)

LIBNTFS3G_OBJS_020 = $(addprefix obj/68020/,$(LIBNTFS3G_SRCS:.c=.o))
LIBDISKIO_OBJS_020 = $(addprefix obj/68020/,$(LIBDISKIO_SRCS:.c=.o))
LIBSUPPORT_OBJS_020 = $(addprefix obj/68020/,$(LIBSUPPORT_SRCS:.c=.o))
OBJS_020 = $(addprefix obj/68020/,$(SRCS:.c=.o))
DEPS_020 = $(LIBNTFS3G_OBJS_020:.o=.d) \
           $(LIBDISKIO_OBJS_020:.o=.d) \
           $(LIBSUPPORT_OBJS_020:.o=.d) \
           $(OBJS_020:.o=.d)

LIBNTFS3G_OBJS_060 = $(addprefix obj/68060/,$(LIBNTFS3G_SRCS:.c=.o))
LIBDISKIO_OBJS_060 = $(addprefix obj/68060/,$(LIBDISKIO_SRCS:.c=.o))
LIBSUPPORT_OBJS_060 = $(addprefix obj/68060/,$(LIBSUPPORT_SRCS:.c=.o))
OBJS_060 = $(addprefix obj/68060/,$(SRCS:.c=.o))
DEPS_060 = $(LIBNTFS3G_OBJS_060:.o=.d) \
           $(LIBDISKIO_OBJS_060:.o=.d) \
           $(LIBSUPPORT_OBJS_060:.o=.d) \
           $(OBJS_060:.o=.d)

.PHONY: all
all: bin/$(TARGET).000 bin/$(TARGET).020 bin/$(TARGET).060

-include $(DEPS_000)
-include $(DEPS_020)
-include $(DEPS_060)

obj/68000/libntfs-3g/%.o: libntfs-3g/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_000) $(CFLAGS) $<
	$(CC) $(ARCH_000) $(CFLAGS) -c -o $@ $<

obj/68020/libntfs-3g/%.o: libntfs-3g/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_020) $(CFLAGS) $<
	$(CC) $(ARCH_020) $(CFLAGS) -c -o $@ $<

obj/68060/libntfs-3g/%.o: libntfs-3g/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_060) $(CFLAGS) $<
	$(CC) $(ARCH_060) $(CFLAGS) -c -o $@ $<

obj/68000/libdiskio/%.o: libdiskio/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_000) $(CFLAGS) $<
	$(CC) $(ARCH_000) $(CFLAGS) -c -o $@ $<

obj/68020/libdiskio/%.o: libdiskio/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_020) $(CFLAGS) $<
	$(CC) $(ARCH_020) $(CFLAGS) -c -o $@ $<

obj/68060/libdiskio/%.o: libdiskio/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_060) $(CFLAGS) $<
	$(CC) $(ARCH_060) $(CFLAGS) -c -o $@ $<

obj/68000/amigaos_support/%.o: amigaos_support/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_000) $(CFLAGS) $<
	$(CC) $(ARCH_000) $(CFLAGS) -c -o $@ $<

obj/68020/amigaos_support/%.o: amigaos_support/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_020) $(CFLAGS) $<
	$(CC) $(ARCH_020) $(CFLAGS) -c -o $@ $<

obj/68060/amigaos_support/%.o: amigaos_support/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_060) $(CFLAGS) $<
	$(CC) $(ARCH_060) $(CFLAGS) -c -o $@ $<

obj/%/amigaos_support/malloc.o: CFLAGS += -fno-builtin

obj/68000/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_000) $(CFLAGS) $<
	$(CC) $(ARCH_000) $(CFLAGS) -c -o $@ $<

obj/68020/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_020) $(CFLAGS) $<
	$(CC) $(ARCH_020) $(CFLAGS) -c -o $@ $<

obj/68060/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_060) $(CFLAGS) $<
	$(CC) $(ARCH_060) $(CFLAGS) -c -o $@ $<

obj/68000/ntfsprogs/%.o: ntfsprogs/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_000) $(CFLAGS) $<
	$(CC) $(ARCH_000) $(CFLAGS) -c -o $@ $<

obj/68020/ntfsprogs/%.o: ntfsprogs/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_020) $(CFLAGS) $<
	$(CC) $(ARCH_020) $(CFLAGS) -c -o $@ $<

obj/68060/ntfsprogs/%.o: ntfsprogs/%.c
	@mkdir -p $(dir $@)
	$(CC) -MM -MP -MT $(@:.o=.d) -MT $@ -MF $(@:.o=.d) $(ARCH_060) $(CFLAGS) $<
	$(CC) $(ARCH_060) $(CFLAGS) -c -o $@ $<

bin/$(LIBNTFS3G).000: $(LIBNTFS3G_OBJS_000)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBNTFS3G).020: $(LIBNTFS3G_OBJS_020)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBNTFS3G).060: $(LIBNTFS3G_OBJS_060)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBDISKIO).000: $(LIBDISKIO_OBJS_000)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBDISKIO).020: $(LIBDISKIO_OBJS_020)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBDISKIO).060: $(LIBDISKIO_OBJS_060)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBSUPPORT).000: $(LIBSUPPORT_OBJS_000)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBSUPPORT).020: $(LIBSUPPORT_OBJS_020)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(LIBSUPPORT).060: $(LIBSUPPORT_OBJS_060)
	@mkdir -p $(dir $@)
	$(AR) -crv $@ $^
	$(RANLIB) $@

bin/$(TARGET).000.debug: $(OBJS_000) bin/$(LIBNTFS3G).000 bin/$(LIBDISKIO).000 bin/$(LIBSUPPORT).000
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_000) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/$(TARGET).020.debug: $(OBJS_020) bin/$(LIBNTFS3G).020 bin/$(LIBDISKIO).020 bin/$(LIBSUPPORT).020
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_020) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/$(TARGET).060.debug: $(OBJS_060) bin/$(LIBNTFS3G).060 bin/$(LIBDISKIO).060 bin/$(LIBSUPPORT).060
	@mkdir -p $(dir $@)
	$(CC) $(ARCH_060) $(LDFLAGS) -o $@ $^ $(LIBS)

bin/$(TARGET).000: bin/$(TARGET).000.debug
	$(STRIP) $(STRIPFLAGS) -o $@ $<

bin/$(TARGET).020: bin/$(TARGET).020.debug
	$(STRIP) $(STRIPFLAGS) -o $@ $<

bin/$(TARGET).060: bin/$(TARGET).060.debug
	$(STRIP) $(STRIPFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf bin obj

.PHONY: revision
revision:
	bumprev $(VERSION) NTFileSystem3G
	bumprev $(VERSION) ntfs3g-handler

