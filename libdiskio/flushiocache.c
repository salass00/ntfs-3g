/**
 * Copyright (c) 2014-2016 Fredrik Wikstrom <fredrik@a500.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "diskio_internal.h"

int DIO_FlushIOCache(struct DiskIO *dio) {
	LONG res = DIO_SUCCESS;

	DEBUGF("DIO_FlushIOCache(%#p)\n", dio);

	if (dio == NULL || dio->disk_ok == FALSE)
		return DIO_ERROR_UNSPECIFIED;

	if (dio->block_cache != NULL && dio->write_cache_enabled) {
		if (FlushDirtyNodes(dio->block_cache, 0) == FALSE) {
			DEBUGF("DIO_FlushIOCache failed\n");
			return DIO_ERROR_UNSPECIFIED;
		}
	}

	if (dio->doupdate) {
		res = DeviceUpdate(dio);
	}

	return res;
}

