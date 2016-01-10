/**
 * Copyright (c) 2015-2016 Fredrik Wikstrom <fredrik@a500.org>
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

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <errno.h>
#include <proto/exec.h>

static APTR mempool;

#define ALLOC_EXTRA_BYTES 0

int setup_malloc(void) {
	mempool = CreatePool(MEMF_ANY, 8192, 2048);
	return mempool != NULL;
}

void cleanup_malloc(void) {
	DeletePool(mempool);
}

void *malloc(size_t size) {
	size_t *pmem = AllocPooled(mempool, size + sizeof(size_t) + ALLOC_EXTRA_BYTES);
	if (pmem != NULL) {
		*pmem++ = size;
	} else
		errno = ENOMEM;
	return pmem;
}

void free(void *ptr) {
	if (ptr != NULL) {
		size_t *pmem = ptr;
		size_t size = *--pmem;
		FreePooled(mempool, pmem, size + sizeof(size_t) + ALLOC_EXTRA_BYTES);
	}
}

static inline size_t get_malloc_size(const void *ptr) {
	if (ptr != NULL) {
		const size_t *pmem = ptr;
		return *--pmem;
	} else {
		return 0;
	}
}

void *calloc(size_t num, size_t size) {
	size *= num;
	void *ptr = malloc(size);
	if (ptr != NULL) bzero(ptr, size);
	return ptr;
}

void *realloc(void *ptr, size_t size) {
	size_t osize;
	void *nptr;
	if (ptr == NULL) return malloc(size);
	osize = get_malloc_size(ptr);
	if (size == osize) return ptr;
	nptr = malloc(size);
	if (nptr != NULL) memcpy(nptr, ptr, MIN(size, osize));
	free(ptr);
	return nptr;
}

