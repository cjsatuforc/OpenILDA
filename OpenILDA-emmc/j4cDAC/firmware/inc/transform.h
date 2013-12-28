/* j4cDAC geometric corrector
 *
 * Copyright 2011 Jacob Potter
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

#ifndef TRANSFORM_MAX

#define COORD_MAX_EXP	15
#define COORD_MAX	(1 << COORD_MAX_EXP)

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <attrib.h>

#define COORD_TOO_CLOSE	150

#define CORNER_TL	0
#define CORNER_TR	1
#define CORNER_BL	2
#define CORNER_BR	3

#define CORNER_FLIP_V(corner)	((corner) ^ 1)
#define CORNER_FLIP_H(corner)	((corner) ^ 2)

#define IS_TOP(corner)		(((corner) & 2) == 0)
#define IS_BOTTOM(corner)	(((corner) & 2) == 2)
#define IS_LEFT(corner)		(((corner) & 1) == 0)
#define IS_RIGHT(corner)	(((corner) & 1) == 1)

typedef int32_t transform[4];

void update_transform(void);

extern int32_t transform_matrix[8];

static inline int32_t ALWAYS_INLINE translate(int32_t *c, int x, int y) {
	int32_t xy_scale = (x * y) >> COORD_MAX_EXP;
	return ((c[0]*x + c[1]*y + c[2]*xy_scale) >> COORD_MAX_EXP) + c[3];
}

static inline int32_t ALWAYS_INLINE translate_x(int32_t x, int32_t y) {
        return translate(transform_matrix, x, y);
}

static inline int32_t ALWAYS_INLINE translate_y(int32_t x, int32_t y) {
        return translate(transform_matrix + 4, x, y);
}

#endif

#endif
