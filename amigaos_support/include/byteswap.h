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

#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#ifdef __AROS__
#include <stdint.h>
#endif
#include <sys/types.h>

#if defined(AMIGA) && !defined(__AROS__) && !defined(STDINT_TYPES_DEFINED)
#define STDINT_TYPES_DEFINED
typedef u_int8_t  uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;
#endif

#if defined(__GNUC__) && (defined(__i386__) || defined(__i486__) || defined(__i586__))

static inline uint16_t bswap_16(uint16_t val) {
	__asm__("xchgb %b0,%h0"
		: "=q" (val)
		:  "0" (val));
	return val;
}

static inline uint32_t bswap_32(uint32_t val) {
	__asm__("bswap %0"
		: "=r" (val)
		: "0" (val));
	return val;
}

static inline uint64_t bswap_64(uint64_t val) {
	return (bswap_32(val >> 32) | (((uint64_t)bswap_32(val & 0xffffffffUL)) << 32));
}

#elif defined(__GNUC__) && defined(__PPC__)

static inline uint16_t bswap_16(uint16_t val) {
	__asm__("rlwinm %0,%1,8,16,23\n\t"
		"rlwimi %0,%1,24,24,31"
		: "=&r" (val)
		: "r" (val));
		return val;
}

static inline uint32_t bswap_32(uint32_t val) {
	__asm__("rlwinm %0,%1,24,0,31\n\t"
		"rlwimi %0,%1,8,8,15\n\t"
		"rlwimi %0,%1,8,24,31"
		: "=&r" (val)
		: "r" (val));
	return val;
}

static inline uint64_t bswap_64(uint64_t val) {
	__asm__("rlwinm %L0,%H1,24,0,31\n\t"
		"rlwinm %H0,%L1,24,0,31\n\t"
		"rlwimi %L0,%H1,8,8,15\n\t"
		"rlwimi %H0,%L1,8,8,15\n\t"
		"rlwimi %L0,%H1,8,24,31\n\t"
		"rlwimi %H0,%L1,8,24,31"
		: "=&r" (val)
		: "r" (val));
	return val;
}

#elif defined(__GNUC__) && (defined(_M68000) || defined(__M68000) || defined(__mc68000) || defined(__M68K__))

static inline uint16_t bswap_16(uint16_t val) {
	__asm__("rolw #8,%0"
		: "=d" (val)
		:  "0" (val));
	return val;
}

static inline uint32_t bswap_32(uint32_t val) {
	__asm__("rolw #8,%0\n\t"
		"swap %0\n\t"
		"rolw #8,%0"
		: "=d" (val)
		: "0" (val));
	return val;
}

static inline uint64_t bswap_64(uint64_t val) {
	return (bswap_32(val >> 32) | (((uint64_t)bswap_32(val & 0xffffffffUL)) << 32));
}

#else

#define bswap_16(x) \
	((uint16_t)((((uint16_t)(x) & 0xff00) >> 8) | \
	            (((uint16_t)(x) & 0x00ff) << 8)))

#define bswap_32(x) \
	((uint32_t)((((uint32_t)(x) & 0xff000000) >> 24) | \
	            (((uint32_t)(x) & 0x00ff0000) >>  8) | \
	            (((uint32_t)(x) & 0x0000ff00) <<  8) | \
	            (((uint32_t)(x) & 0x000000ff) << 24)))

#define bswap_64(x) \
	((uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
	            (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
	            (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
	            (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
	            (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
	            (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
	            (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
	            (((uint64_t)(x) & 0x00000000000000ffULL) << 56)))

#endif

#endif

