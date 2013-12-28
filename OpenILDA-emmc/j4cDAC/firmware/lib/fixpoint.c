/* Fixed point math utilities
 *
 * Copyright 2011 Dan Mills
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include "fixpoint.h"

static const int16_t SINE [257] = {
	0,	804,	1608,	2410,
	3212,	4011,	4808,	5602,
	6393,	7179,	7962,	8739,
	9512,	10278,	11039,	11793,
	12539,	13279,	14010,	14732,
	15446,	16151,	16846,	17530,
	18204,	18868,	19519,	20159,
	20787,	21403,	22005,	22594,
	23170,	23731,	24279,	24811,
	25329,	25832,	26319,	26790,
	27245,	27683,	28105,	28510,
	28898,	29268,	29621,	29956,
	30273,	30571,	30852,	31113,
	31356,	31580,	31785,	31971,
	32137,	32285,	32412,	32521,
	32609,	32678,	32728,	32757,
	32767,	32757,	32728,	32678,
	32609,	32521,	32412,	32285,
	32137,	31971,	31785,	31580,
	31356,	31113,	30852,	30571,
	30273,	29956,	29621,	29268,
	28898,	28510,	28105,	27683,
	27245,	26790,	26319,	25832,
	25329,	24811,	24279,	23731,
	23170,	22594,	22005,	21403,
	20787,	20159,	19519,	18868,
	18204,	17530,	16846,	16151,
	15446,	14732,	14010,	13279,
	12539,	11793,	11039,	10278,
	9512,	8739,	7962,	7179,
	6393,	5602,	4808,	4011,
	3212,	2410,	1608,	804,
	0,	-804,	-1608,	-2410,
	-3212,	-4011,	-4808,	-5602,
	-6393,	-7179,	-7962,	-8739,
	-9512,	-10278,	-11039,	-11793,
	-12539,	-13279,	-14010,	-14732,
	-15446,	-16151,	-16846,	-17530,
	-18204,	-18868,	-19519,	-20159,
	-20787,	-21403,	-22005,	-22594,
	-23170,	-23731,	-24279,	-24811,
	-25329,	-25832,	-26319,	-26790,
	-27245,	-27683,	-28105,	-28510,
	-28898,	-29268,	-29621,	-29956,
	-30273,	-30571,	-30852,	-31113,
	-31356,	-31580,	-31785,	-31971,
	-32137,	-32285,	-32412,	-32521,
	-32609,	-32678,	-32728,	-32757,
	-32767,	-32757,	-32728,	-32678,
	-32609,	-32521,	-32412,	-32285,
	-32137,	-31971,	-31785,	-31580,
	-31356,	-31113,	-30852,	-30571,
	-30273,	-29956,	-29621,	-29268,
	-28898,	-28510,	-28105,	-27683,
	-27245,	-26790,	-26319,	-25832,
	-25329,	-24811,	-24279,	-23731,
	-23170,	-22594,	-22005,	-21403,
	-20787,	-20159,	-19519,	-18868,
	-18204,	-17530,	-16846,	-16151,
	-15446,	-14732,	-14010,	-13279,
	-12539,	-11793,	-11039,	-10278,
	-9512,	-8739,	-7962,	-7179,
	-6393,	-5602,	-4808,	-4011,
	-3212,	-2410,	-1608,	-804, 0 
};

static inline fixed interpolatewave (const uint32_t phase, const int16_t * const waveform)
{
	int32_t v1, v2;
	const uint32_t index = (phase >> 24);// 0 -> 255
	const uint32_t frac = (phase & 0x00ffffff)>>8; // Avoids risking overflow
	int32_t delta;
	v1 = waveform[index];
	v2 = waveform[index+1];

	// note that this may = 256 hence why there are 257 values in the LUT
	delta = v2 - v1; 
	v1 = v1 << 16;
	delta *=frac;

	// frac is in 0 -> ffff, so this means that (delta >>8) + v1 = 32 bit result.
	return (v1+delta)>>(INTEGER - 1); // scale to +- 1.0
}

fixed fix_sine (const uint32_t phase)
{
	return interpolatewave (phase,SINE);
}

/* strtofixed
 *
 * Interpret a string (like "123.456") as a fixed-point value.
 *
 * In addition to the standard decimal notation, this also recognizes a
 * trailing % sign to mean that a value should be divided by 100.
 */
fixed strtofixed(const char *c) {
	if (!c) return 0;

	int32_t integer = 0, num = 0, denom = 1;

	/* Recognize a leading minus sign */
	if (*c == '-') {
		denom = -1;
		c++;
	}

	/* Parse the integer part */
	while (*c >= '0' && *c <= '9') {
		integer *= 10;
		integer += *c - '0';
		c++;
	}

	/* Special: handle percent */
	if (*c == '%') {
		return FIXED(integer) * denom / 100;
	} else if (*c != '.') {
		return FIXED(integer) * denom;
	}

	c++;

	/* Parse fractional part */
	while (*c >= '0' && *c <= '9' && denom < 10000 && denom > -10000) {
		denom *= 10;
		num *= 10;
		num += *c - '0';
		c++;
	}

	num <<= POINT;

	/* Round appropriately */
	if (num % denom > (denom / 2))
		return FIXED(integer) + (num / denom) + 1;
	else
		return FIXED(integer) + num / denom;
}
