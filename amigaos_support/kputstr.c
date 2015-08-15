/**
 * Copyright (c) 2015 Fredrik Wikstrom <fredrik@a500.org>
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

#include <clib/debug_protos.h>

#if !defined(__AROS__) && !defined(NODEBUG)
void KPutStr(CONST_STRPTR str) {
	__asm__ __volatile__
	(
		"move.l 4.w,a6\n\t"
		"bra.s 2f\n"
		"1:\n\t"
		"jsr -516(a6)\n"
		"2:\n\t"
		"move.b (%0)+,d0\n\t"
		"bne.s 1b"
		:
		: "a" (str)
		: "d0", "a6"
	);
}
#endif

