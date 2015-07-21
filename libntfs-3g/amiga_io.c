/*
 * amiga_io.c --- DiskIO cached version of Amiga device I/O backend.
 *
 * Copyright (C) 2013-2015 Fredrik Wikstrom.
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "diskio.h"
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "device.h"

extern const char *EXEC_NAME;

#define SET_DEV_PD(dev,pd) dev->d_private = pd
#define DEV_PD(dev) dev->d_private

//#define DBG_DEV_OPS
#include <debugf.h>
#ifdef DBG_DEV_OPS
	#define DEBUGF(str,args...) debugf("dev->ops: " str, ## args)
#else
	#define DEBUGF(str,args...)
#endif

struct amiga_private_data {
	struct DiskIO *diskio;
	ULONG sector_size;
	QUAD byte_pos;
	UQUAD total_size;
};

static int ntfs_device_amiga_io_open(struct ntfs_device *dev, int flags) {
	struct amiga_private_data *data = NULL;
	ULONG disk_present, write_protected, disk_ok, sector_size;
	UQUAD total_sectors;

	DEBUGF("ntfs_device_amiga_io_open(0x%x, 0x%x)\n", dev, flags);

	if (NDevOpen(dev)) {
		errno = EBUSY;
		return -1;
	}

	if ((flags & O_RDWR) != O_RDWR)
		NDevSetReadOnly(dev);

	data = malloc(sizeof(*data));
	if (data == NULL) {
		errno = ENOMEM;
		goto error;
	}

	if (strcmp(EXEC_NAME, "ntfs3g-handler") == 0) {
		data->diskio = DIO_Setup((CONST_STRPTR)dev->d_name, NULL);
	} else {
		data->diskio = DIO_SetupTags((CONST_STRPTR)dev->d_name,
			DIOS_DOSType, ID_NTFS_DISK,
			DIOS_Inhibit, TRUE,
			TAG_END);
	}
	if (data->diskio == NULL) {
		errno = ENODEV;
		goto error;
	}

	DIO_QueryTags(data->diskio,
		DIOQ_DiskPresent,    &disk_present,
		DIOQ_WriteProtected, &write_protected,
		DIOQ_DiskValid,      &disk_ok,
		DIOQ_TotalSectors,   &total_sectors,
		DIOQ_BytesPerSector, &sector_size,
		TAG_END);

	if (!disk_present || !disk_ok) {
		errno = ENODEV;
		goto error;
	}

	if (write_protected && (flags & O_RDWR) == O_RDWR) {
		errno = EROFS;
		goto error;
	}

	data->sector_size = sector_size;
	data->byte_pos = 0;
	data->total_size = total_sectors * sector_size;

	SET_DEV_PD(dev, data);

	DEBUGF("ntfs_device_amiga_io_open done.\n"
		"   sectors %lld sectorsize %ld blocksize %ld totalsize %lld\n",
		total_sectors, sector_size, sector_size, data->total_size);

	NDevSetOpen(dev);
	return 0;

error:
	DEBUGF("ntfs_device_amiga_io_open failed: 0x%x\n", errno);

	if (data != NULL) {
		DIO_Cleanup(data->diskio);
		free(data);
	}

	return -1;
}

static int ntfs_device_amiga_io_sync(struct ntfs_device *dev);

static int ntfs_device_amiga_io_close(struct ntfs_device *dev) {
	DEBUGF("ntfs_device_amiga_io_close(0x%x)\n", dev);
	int result = 0;

	if (!NDevOpen(dev)) {
		errno = EBADF;
		ntfs_log_perror("Device %s is not open", dev->d_name);
		return -1;
	}

	struct amiga_private_data *data = DEV_PD(dev);

	if (NDevDirty(dev))
		result = ntfs_device_amiga_io_sync(dev);

	DIO_Cleanup(data->diskio);
	free(data);

	SET_DEV_PD(dev, NULL);

	NDevClearOpen(dev);
	return result;
}

static QUAD ntfs_device_amiga_io_seek(struct ntfs_device *dev, QUAD offset, int whence) {
	DEBUGF("ntfs_device_amiga_io_seek(0x%x, %lld, %ld)\n", dev, offset, whence);

	struct amiga_private_data *data = DEV_PD(dev);

	QUAD old_pos = data->byte_pos;

	switch (whence) {
	case SEEK_SET:
		data->byte_pos = offset;
		break;
	case SEEK_CUR:
		data->byte_pos += offset;
		break;
	case SEEK_END:
		data->byte_pos = data->total_size + offset;
		break;
	default:
		errno = EINVAL;
		goto error;
	}

	if (data->byte_pos < 0 || data->byte_pos > data->total_size) {
		errno = ESPIPE;
		goto error;
	}

	return data->byte_pos;

error:
	DEBUGF("ntfs_device_amiga_io_seek failed: 0x%x\n", errno);
	data->byte_pos = old_pos;
	return -1;
}

static int amiga_read(struct amiga_private_data *data, QUAD offset, APTR buffer, QUAD count) {
	if (count < 0 || count > UINT32_MAX)
		return EINVAL;
	else if (DIO_ReadBytes(data->diskio, offset, buffer, (ULONG)count) != 0)
		return ESPIPE;
	else
		return 0;
}

static int amiga_write(struct amiga_private_data *data, QUAD offset, CONST_APTR buffer, QUAD count) {
	if (count < 0 || count > UINT32_MAX)
		return EINVAL;
	else if (DIO_WriteBytes(data->diskio, offset, buffer, (ULONG)count) != 0)
		return ESPIPE;
	else
		return 0;
}

static QUAD ntfs_device_amiga_io_read(struct ntfs_device *dev, APTR buffer, QUAD count) {
	DEBUGF("ntfs_device_amiga_io_read(0x%x, 0x%x, %lld)\n", dev, buffer, count);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_read(data, data->byte_pos, buffer, count);
	if (errno) goto error;

	data->byte_pos += count;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_read failed 0x%x\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_write(struct ntfs_device *dev, CONST_APTR buffer, QUAD count) {
	DEBUGF("ntfs_device_amiga_io_write(0x%x, 0x%x, %lld)\n", dev, buffer, count);

	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	NDevSetDirty(dev);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_write(data, data->byte_pos, buffer, count);
	if (errno) goto error;

	data->byte_pos += count;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_write failed: 0x%x\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_pread(struct ntfs_device *dev, APTR buffer, QUAD count, QUAD offset) {
	DEBUGF("ntfs_device_amiga_io_pread(0x%x, 0x%x, %lld, %lld)\n", dev, buffer, count, offset);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_read(data, offset, buffer, count);
	if (errno) goto error;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_pread failed: 0x%x\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_pwrite(struct ntfs_device *dev, CONST_APTR buffer, QUAD count, QUAD offset) {
	DEBUGF("ntfs_device_amiga_io_pwrite(0x%x, 0x%x, %lld, %lld)\n", dev, buffer, count, offset);

	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}
	NDevSetDirty(dev);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_write(data, offset, buffer, count);
	if (errno) goto error;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_pwrite failed: 0x%x\n", errno);
	return -1;
}

static int ntfs_device_amiga_io_sync(struct ntfs_device *dev) {
	DEBUGF("ntfs_device_amiga_io_sync(0x%x)\n", dev);

	struct amiga_private_data *data = DEV_PD(dev);

	if (!NDevReadOnly(dev)) {
		if (DIO_FlushIOCache(data->diskio) != 0) {
			ntfs_log_perror("Failed to sync device %s", dev->d_name);
			return -1;
		}
		NDevClearDirty(dev);
	}

	return 0;
}

static int ntfs_device_amiga_io_stat(struct ntfs_device *dev, struct fbx_stat *buf) {
	DEBUGF("ntfs_device_amiga_io_stat(0x%x, 0x%x)\n", dev, stat);

	struct amiga_private_data *data = DEV_PD(dev);

	buf->st_size = data->total_size;
	buf->st_blocks = data->total_size / data->sector_size;
	buf->st_blksize = data->sector_size;
	buf->st_mode = S_IFBLK;

	return 0;
}

static int ntfs_device_amiga_io_ioctl(struct ntfs_device *dev, int request, void *argp) {
	DEBUGF("ntfs_device_amiga_io_ioctl(0x%x, %ld, 0x%x)\n", dev, request, argp);

	struct amiga_private_data *data = DEV_PD(dev);
	struct hd_geometry *hdg;

	switch (request) {
	case BLKGETSIZE64:
		*(UQUAD *)argp = data->total_size;
		break;
	case BLKSSZGET:
		*(int *)argp = data->sector_size;
		break;
	case BLKBSZSET:
		// do nothing. just succeed.
		break;
	case HDIO_GETGEO:
		hdg = argp;
		hdg->start = 0;
		// just put some dummy values.
		hdg->heads = 1;
		hdg->sectors = 1;
		hdg->cylinders = 1;
		break;
	default:
		errno = EINVAL;
		goto error;
	}

	return 0;

error:
	DEBUGF("ntfs_device_amiga_io_ioctl failed: 0x%x\n", errno);
	return -1;
}

struct ntfs_device_operations ntfs_device_amiga_io_ops = {
	.open   = ntfs_device_amiga_io_open,
	.close  = ntfs_device_amiga_io_close,
	.seek   = ntfs_device_amiga_io_seek,
	.read   = ntfs_device_amiga_io_read,
	.write  = ntfs_device_amiga_io_write,
	.pread  = ntfs_device_amiga_io_pread,
	.pwrite = ntfs_device_amiga_io_pwrite,
	.sync   = ntfs_device_amiga_io_sync,
	.stat   = ntfs_device_amiga_io_stat,
	.ioctl  = ntfs_device_amiga_io_ioctl
};

