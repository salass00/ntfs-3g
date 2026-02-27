/**
 * Copyright (c) 2015-2026 Fredrik Wikstrom <fredrik@a500.org>
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

#include <devices/timer.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <proto/exec.h>
#include <proto/timer.h>

#ifndef NEWLIST
#define NEWLIST(list) \
	do { \
		(list)->lh_Head = (struct Node *)&(list)->lh_Tail; \
		(list)->lh_Tail = NULL; \
		(list)->lh_TailPred = (struct Node *)&(list)->lh_Head; \
	} while (0)
#endif

#ifndef __AROS__
int gettimeofday(struct timeval *tv, struct timezone *tz) {
	struct MsgPort mp;
	struct timerequest tr;
	struct Device *TimerBase;

	bzero(&mp, sizeof(mp));
	bzero(&tr, sizeof(tr));

	mp.mp_Node.ln_Type    = NT_MSGPORT;
	mp.mp_Flags           = PA_IGNORE;
	mp.mp_MsgList.lh_Type = NT_MESSAGE;
	NEWLIST(&mp.mp_MsgList);

	tr.tr_node.io_Message.mn_Node.ln_Type = NT_MESSAGE;
	tr.tr_node.io_Message.mn_ReplyPort    = &mp;
	tr.tr_node.io_Message.mn_Length       = sizeof(tr);

	if (OpenDevice("timer.device", UNIT_VBLANK, &tr.tr_node, 0) != 0) {
		errno = ENOMEM;
		return -1;
	}

	TimerBase = tr.tr_node.io_Device;
	GetSysTime(tv);

	CloseDevice(&tr.tr_node);

	return 0;
}
#endif

