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

#ifndef __AROS__
#include <grp.h>
#include <errno.h>

__stdargs struct group *getgrnam(const char *name) {
	errno = 0;
	return NULL;
}

__stdargs struct group *getgrgid(gid_t gid) {
	errno = 0;
	return NULL;
}

__stdargs struct passwd *getpwnam(const char *name) {
	errno = 0;
	return NULL;
}

__stdargs struct passwd *getpwuid(uid_t uid) {
	errno = 0;
	return NULL;
}
#endif
