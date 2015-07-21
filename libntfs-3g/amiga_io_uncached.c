/*
 * amiga_io.c --- Uncached version of Amiga device I/O backend.
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

#include <devices/trackdisk.h>
#include <devices/newstyle.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/filesysbox.h>
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
	struct IOExtTD *iotd;
	UBYTE *sector_buffer;

	UWORD read_cmd;
	UWORD write_cmd;

	ULONG disk_id;
	LONG sector_size;
	QUAD start_sector, num_sectors;
	QUAD start_byte, num_bytes;
	QUAD byte_pos;

	const char *devname;

	struct NSDeviceQueryResult qr;
	struct DriveGeometry dg;
};

static BOOL amiga_read_sectors_uncached(struct amiga_private_data *data, QUAD sector, LONG num_sectors, APTR buffer) {
	DEBUGF("amiga_read_sectors_uncached(%#p, %lld, %ld, %#p)\n", data, sector, num_sectors, buffer);

	if (sector < 0 || (sector + num_sectors) > data->num_sectors) {
		return FALSE;
	}

	QUAD offset = (data->start_sector + sector) * data->sector_size;
	struct IOExtTD *iotd = data->iotd;

	iotd->iotd_Req.io_Command = data->read_cmd;
	iotd->iotd_Req.io_Data = buffer;
	iotd->iotd_Req.io_Actual = offset >> 32;
	iotd->iotd_Req.io_Offset = offset;
	iotd->iotd_Req.io_Length = num_sectors * data->sector_size;
	iotd->iotd_Count = data->disk_id;

	if (DoIO((struct IORequest *)iotd) != 0) {
		debugf("%s/%s: read error! offset: %lld length: %ld error: %d\n",
			data->devname, EXEC_NAME, offset, num_sectors * data->sector_size,
			iotd->iotd_Req.io_Error);
		return FALSE;
	}
	return TRUE;
}

static BOOL amiga_write_sectors_uncached(struct amiga_private_data *data, QUAD sector, LONG num_sectors, CONST_APTR buffer) {
	DEBUGF("amiga_write_sectors_uncached(%#p, %lld, %ld, %#p)\n", data, sector, num_sectors, buffer);

	if (sector < 0 || (sector + num_sectors) > data->num_sectors) {
		return FALSE;
	}

	QUAD offset = (data->start_sector + sector) * data->sector_size;
	struct IOExtTD *iotd = data->iotd;

	iotd->iotd_Req.io_Command = data->write_cmd;
	iotd->iotd_Req.io_Data = (APTR)buffer;
	iotd->iotd_Req.io_Actual = offset >> 32;
	iotd->iotd_Req.io_Offset = offset;
	iotd->iotd_Req.io_Length = num_sectors * data->sector_size;
	iotd->iotd_Count = data->disk_id;

	if (DoIO((struct IORequest *)iotd) != 0) {
		debugf("%s/%s: write error! offset: %lld length: %ld error: %ld\n",
			data->devname, EXEC_NAME, offset, num_sectors * data->sector_size,
			iotd->iotd_Req.io_Error);
		return FALSE;
	}
	return TRUE;
}

static int ntfs_device_amiga_io_open(struct ntfs_device *dev, int flags) {
	struct DosList *dol;
	struct DeviceNode *dn = NULL;
	struct FileSysStartupMsg *fssm = NULL;
	struct DosEnvec *de = NULL;
	struct amiga_private_data *data = NULL;
	struct MsgPort *mp = NULL;
	struct IOExtTD *iotd = NULL;
	struct NSDeviceQueryResult *qr;
	const UWORD *cmdp;
	BOOL etd64 = FALSE;
	BOOL nsd64 = FALSE;
	BOOL etd = FALSE;
	struct DriveGeometry *dg;
	QUAD end_byte;
	char devname[256];

	DEBUGF("ntfs_device_amiga_io_open(%#p, %#x)\n", dev, flags);

	if (NDevOpen(dev)) {
		errno = EBUSY;
		return -1;
	}

	if ((flags & O_RDWR) != O_RDWR)
		NDevSetReadOnly(dev);

	dol = LockDosList(LDF_DEVICES|LDF_READ);
	dn = (struct DeviceNode *)FindDosEntry(dol, (CONST_STRPTR)dev->d_name, LDF_DEVICES|LDF_READ);
	UnLockDosList(LDF_DEVICES|LDF_READ);
	if (dn == NULL) {
		DEBUGF("device node '%s' not found!\n", dev->d_name);
		errno = ENODEV;
		goto error;
	}

	if ((fssm = BADDR(dn->dn_Startup)) == NULL ||
		(de = BADDR(fssm->fssm_Environ)) == NULL ||
		de->de_TableSize < DE_UPPERCYL)
	{
		errno = ENOMSG;
		goto error;
	}

	data = AllocMem(sizeof(*data), MEMF_ANY|MEMF_CLEAR);
	if (data == NULL) {
		errno = ENOMEM;
		goto error;
	}

#ifdef __AROS__
	data->devname = (const char *)AROS_BSTR_ADDR(dn->dn_Name);
#else
	data->devname = (const char *)BADDR(dn->dn_Name) + 1;
#endif

	mp = CreateMsgPort();
	iotd = CreateIORequest(mp, sizeof(*iotd));
	if (iotd == NULL) {
		errno = ENOMEM;
		goto error;
	}

	FbxCopyStringBSTRToC(fssm->fssm_Device, (STRPTR)devname, sizeof(devname));
	if (OpenDevice((CONST_STRPTR)devname, fssm->fssm_Unit,
		(struct IORequest *)iotd, fssm->fssm_Flags) != 0)
	{
		DEBUGF("OpenDevice() failed: %d\n", iotd->iotd_Req.io_Error);
		DEBUGF("Device: '%s' Unit: %ld Flags: 0x%lx\n",
			devname, fssm->fssm_Unit, fssm->fssm_Flags);
		iotd->iotd_Req.io_Device = NULL;
		errno = ENODEV;
		goto error;
	}

	data->iotd = iotd;

	qr = &data->qr;
	iotd->iotd_Req.io_Command = NSCMD_DEVICEQUERY;
	iotd->iotd_Req.io_Data = qr;
	iotd->iotd_Req.io_Length = sizeof(*qr);
	if (DoIO((struct IORequest *)iotd) != 0) {
		DEBUGF("NSCMD_DEVICEQUERY failed: %d\n", iotd->iotd_Req.io_Error);
		errno = EINVAL;
		goto error;
	}
	if (qr->DeviceType != NSDEVTYPE_TRACKDISK) {
		DEBUGF("unsupported device type: %ld\n", qr->DeviceType);
#ifdef ENOTBLK
		errno = ENOTBLK;
#else
		errno = EINVAL;
#endif
		goto error;
	}
	if ((cmdp = qr->SupportedCommands) != NULL) {
		UWORD cmd;
		while ((cmd = *cmdp++) != 0) {
			if (cmd == NSCMD_ETD_READ64) etd64 = TRUE;
			else if (cmd == NSCMD_TD_READ64) nsd64 = TRUE;
			else if (cmd == ETD_READ) etd = TRUE;
		}
	}
	if (etd64) {
		data->read_cmd = NSCMD_ETD_READ64;
		data->write_cmd = NSCMD_ETD_WRITE64;
	} else if (nsd64) {
		data->read_cmd = NSCMD_TD_READ64;
		data->write_cmd = NSCMD_TD_WRITE64;
	} else if (etd) {
		data->read_cmd = ETD_READ;
		data->write_cmd = ETD_WRITE;
	} else {
		data->read_cmd = CMD_READ;
		data->write_cmd = CMD_WRITE;
	}

	iotd->iotd_Req.io_Command = TD_CHANGESTATE;
	if (DoIO((struct IORequest *)iotd) != 0) {
		DEBUGF("TD_CHANGESTATE failed: %d\n", iotd->iotd_Req.io_Error);
		errno = EBUSY;
		goto error;
	}
	if (iotd->iotd_Req.io_Actual != 0) {
		/* No disk in drive */
		DEBUGF("no disk in drive\n");
		errno = ENODEV;
		goto error;
	}

	iotd->iotd_Req.io_Command = TD_CHANGENUM;
	if (DoIO((struct IORequest *)iotd) != 0) {
		DEBUGF("TD_CHANGENUM failed: %d\n", iotd->iotd_Req.io_Error);
		errno = EBUSY;
		goto error;
	}
	data->disk_id = iotd->iotd_Req.io_Actual;

	iotd->iotd_Req.io_Command = TD_PROTSTATUS;
	if (DoIO((struct IORequest *)iotd) != 0) {
		DEBUGF("TD_PROTSTATUS failed: %d\n", iotd->iotd_Req.io_Error);
		errno = EBUSY;
		goto error;
	}
	if (iotd->iotd_Req.io_Actual != 0 && (flags & O_RDWR) == O_RDWR) {
		errno = EROFS;
		goto error;
	}

	dg = &data->dg;
	iotd->iotd_Req.io_Command = TD_GETGEOMETRY;
	iotd->iotd_Req.io_Data = dg;
	iotd->iotd_Req.io_Length = sizeof(*dg);
	if (DoIO((struct IORequest *)iotd) != 0) {
		errno = EBUSY;
		goto error;
	}

	if (de->de_LowCyl == 0) {
		data->sector_size = dg->dg_SectorSize;
		data->start_sector = 0;
		data->num_sectors = (UQUAD)dg->dg_CylSectors * (UQUAD)dg->dg_Cylinders;
	} else {
		ULONG cylsec = de->de_BlocksPerTrack * de->de_Surfaces;
		data->sector_size = de->de_SizeBlock * sizeof(ULONG);
		data->start_sector = (UQUAD)cylsec * (UQUAD)de->de_LowCyl;
		data->num_sectors = (UQUAD)cylsec * (UQUAD)(de->de_HighCyl - de->de_LowCyl + 1);
	}
	data->start_byte = data->start_sector * data->sector_size;
	data->num_bytes = data->num_sectors * data->sector_size;

	end_byte = data->start_byte + data->num_bytes;
	if (!etd64 && !nsd64 && end_byte > (1LL << 32)) {
		errno = ENOSYS;
		goto error;
	}

	data->sector_buffer = AllocMem(data->sector_size, MEMF_PUBLIC);
	if (data->sector_buffer == NULL) {
		errno = ENOMEM;
		goto error;
	}

	data->byte_pos = 0;

	SET_DEV_PD(dev, data);

	DEBUGF("ntfs_device_amiga_io_open done.\n"
		"   sectors %lld sectorsize %ld blocksize %ld totalsize %lld\n",
		data->num_sectors, data->sector_size, data->sector_size, data->num_bytes);

	NDevSetOpen(dev);
	return 0;

error:
	DEBUGF("ntfs_device_amiga_io_open failed: %d\n", errno);

	if (iotd != NULL && iotd->iotd_Req.io_Device != NULL) {
		CloseDevice((struct IORequest *)iotd);
	}
	DeleteIORequest((struct IORequest *)iotd);
	DeleteMsgPort(mp);
	if (data != NULL) {
		if (data->sector_buffer != NULL)
			FreeMem(data->sector_buffer, data->sector_size);
		FreeMem(data, sizeof(*data));
	}

	return -1;
}

static int ntfs_device_amiga_io_close(struct ntfs_device *dev) {
	DEBUGF("ntfs_device_amiga_io_close(%#p)\n", dev);

	struct amiga_private_data *data = DEV_PD(dev);

	if (!NDevOpen(dev)) {
		errno = EBADF;
		ntfs_log_perror("Device '%s' is not open", data->devname);
		return -1;
	}

	FreeMem(data->sector_buffer, data->sector_size);

	struct IOExtTD *iotd = data->iotd;
	struct MsgPort *mp = iotd->iotd_Req.io_Message.mn_ReplyPort;

	CloseDevice((struct IORequest *)iotd);
	DeleteIORequest((struct IORequest *)iotd);
	DeleteMsgPort(mp);
	FreeMem(data, sizeof(*data));

	SET_DEV_PD(dev, NULL);

	NDevClearOpen(dev);
	return 0;
}

static QUAD ntfs_device_amiga_io_seek(struct ntfs_device *dev, QUAD offset, int whence) {
	DEBUGF("ntfs_device_amiga_io_seek(%#p, %lld, %d)\n", dev, offset, whence);

	struct amiga_private_data *data = DEV_PD(dev);

	QUAD oldpos = data->byte_pos;

	switch (whence) {
	case SEEK_SET:
		data->byte_pos = offset;
		break;
	case SEEK_CUR:
		data->byte_pos += offset;
		break;
	case SEEK_END:
		data->byte_pos = data->num_bytes + offset;
		break;
	default:
		errno = EINVAL;
		goto error;
	}

	if (data->byte_pos < 0 || data->byte_pos > data->num_bytes) {
		errno = ESPIPE;
		goto error;
	}

	return data->byte_pos;

error:
	DEBUGF("ntfs_device_amiga_io_seek failed: %d\n", errno);
	data->byte_pos = oldpos;
	return -1;
}

static int amiga_read(struct amiga_private_data *data, QUAD offset, APTR buffer, QUAD len) {
	struct MsgPort *mp = data->iotd->iotd_Req.io_Message.mn_ReplyPort;
	mp->mp_SigTask = FindTask(NULL);

	const LONG mask = data->sector_size - 1;
	const LONG m = (ULONG)offset & mask;
	QUAD sector = offset / data->sector_size;

	if (m > 0) {
		const LONG l = min(data->sector_size - m, len);
		if (!amiga_read_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
		CopyMem(data->sector_buffer + m, buffer, l);
		buffer += l;
		sector++;
		len -= l;
	}

	if (len >= data->sector_size) {
		const LONG num_sectors = len / data->sector_size;
		const LONG l = num_sectors * data->sector_size;
		if (!amiga_read_sectors_uncached(data, sector, num_sectors, buffer)) {
			return ESPIPE;
		}
		buffer += l;
		sector += num_sectors;
		len -= l;
	}

	if (len > 0) {
		const LONG l = len;
		if (!amiga_read_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
		CopyMem(data->sector_buffer, buffer, l);
	}

	return 0;
}

static int amiga_write(struct amiga_private_data *data, QUAD offset, CONST_APTR buffer, QUAD len) {
	struct MsgPort *mp = data->iotd->iotd_Req.io_Message.mn_ReplyPort;
	mp->mp_SigTask = FindTask(NULL);

	const LONG mask = data->sector_size - 1;
	const LONG m = (ULONG)offset & mask;
	QUAD sector = offset / data->sector_size;

	if (m > 0) {
		const LONG l = min(data->sector_size - m, len);
		if (!amiga_read_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
		CopyMem(buffer, data->sector_buffer + m, l);
		if (!amiga_write_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
		buffer += l;
		sector++;
		len -= l;
	}

	if (len >= data->sector_size) {
		const LONG num_sectors = len / data->sector_size;
		const LONG l = num_sectors * data->sector_size;
		if (!amiga_write_sectors_uncached(data, sector, num_sectors, buffer)) {
			return ESPIPE;
		}
		buffer += l;
		sector += num_sectors;
		len -= l;
	}

	if (len > 0) {
		const LONG l = len;
		if (!amiga_read_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
		CopyMem(buffer, data->sector_buffer, l);
		if (!amiga_write_sectors_uncached(data, sector, 1, data->sector_buffer)) {
			return ESPIPE;
		}
	}

	return 0;
}

static QUAD ntfs_device_amiga_io_read(struct ntfs_device *dev, APTR buf, QUAD count) {
	DEBUGF("ntfs_device_amiga_io_read(%#p, %#p, %lld)\n", dev, buf, count);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_read(data, data->byte_pos, buf, count);
	if (errno) goto error;

	data->byte_pos += count;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_read failed: %d\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_write(struct ntfs_device *dev, CONST_APTR buf, QUAD count) {
	DEBUGF("ntfs_device_amiga_io_write(%#p, %#p, %lld)\n", dev, buf, count);

	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_write(data, data->byte_pos, buf, count);
	if (errno) goto error;

	data->byte_pos += count;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_write failed: %d\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_pread(struct ntfs_device *dev, APTR buf, QUAD count, QUAD offset) {
	DEBUGF("ntfs_device_amiga_io_pread(%#p, %#p, %lld, %lld)\n", dev, buf, count, offset);

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_read(data, offset, buf, count);
	if (errno) goto error;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_pread failed: %d\n", errno);
	return -1;
}

static QUAD ntfs_device_amiga_io_pwrite(struct ntfs_device *dev, CONST_APTR buf, QUAD count, QUAD offset) {
	DEBUGF("ntfs_device_amiga_io_pwrite(%#p, %#p, %lld, %lld)\n", dev, buf, count, offset);

	if (NDevReadOnly(dev)) {
		errno = EROFS;
		return -1;
	}

	struct amiga_private_data *data = DEV_PD(dev);

	errno = amiga_write(data, offset, buf, count);
	if (errno) goto error;

	return count;

error:
	DEBUGF("ntfs_device_amiga_io_pwrite failed: %d\n", errno);
	return -1;
}

static int ntfs_device_amiga_io_sync(struct ntfs_device *dev) {
	DEBUGF("ntfs_device_amiga_io_sync(%#p)\n", dev);

	return 0;
}

static int ntfs_device_amiga_io_stat(struct ntfs_device *dev, struct fbx_stat *buf) {
	DEBUGF("ntfs_device_amiga_io_stat(%#p, %#p)\n", dev, stat);

	struct amiga_private_data *data = DEV_PD(dev);

	buf->st_size = data->num_bytes;
	buf->st_blocks = data->num_sectors;
	buf->st_blksize = data->sector_size;
	buf->st_mode = S_IFBLK;

	return 0;
}

static int ntfs_device_amiga_io_ioctl(struct ntfs_device *dev, int request, void *argp) {
	DEBUGF("ntfs_device_amiga_io_ioctl(%#p, %d, %#p)\n", dev, request, argp);

	struct amiga_private_data *data = DEV_PD(dev);
	struct hd_geometry *hdg;

	switch (request) {
	case BLKGETSIZE64:
		*(QUAD *)argp = data->num_bytes;
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
	DEBUGF("ntfs_device_amiga_io_ioctl failed: %d\n", errno);
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

