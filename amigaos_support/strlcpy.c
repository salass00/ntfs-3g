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

#include <string.h>

#ifndef __AROS__
size_t strlcpy(char *dst, const char *src, size_t dst_size) {
	char *dst_start = dst;
	char *dst_end = dst_start + dst_size;

	if (dst_end > dst) {
		while ((*dst = *src) != '\0') {
			if (++dst == dst_end) {
				*--dst = '\0';
				break;
			}
			src++;
		}
	}
	dst += strlen(src);
	return dst - dst_start;
}

size_t strlcat(char *dst, const char *src, size_t dst_size) {
	char *dst_start = dst;
	char *dst_end = dst_start + dst_size;

	dst += strlen(dst);
	if (dst_end > dst) {
		while ((*dst = *src) != '\0') {
			if (++dst == dst_end) {
				*--dst = '\0';
				break;
			}
			src++;
		}
	}
	dst += strlen(src);
	return dst - dst_start;
}
#endif

