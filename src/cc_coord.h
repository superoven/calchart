/* CC_Coord.h
 * Definitions for the CC_Coord classes
 *
 * Modification history:
 * 1-2-95     Garrick Meeker              Created from previous CalPrint
 * 4-16-95    Garrick Meeker              Converted to C++
 *
 */

/*
   Copyright (C) 1994-2008  Garrick Brian Meeker

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CC_COORD_H_
#define _CC_COORD_H_

#ifdef __GNUG__
#pragma interface
#endif

#include "cc_types.h"

#include <stdint.h>

#define COORD_SHIFT 4
#define COORD_DECIMAL (1<<COORD_SHIFT)
#define INT2COORD(a) ((a) * COORD_DECIMAL)
#define COORD2INT(a) ((a) / COORD_DECIMAL)
#define FLOAT2NUM(a) CLIPFLOAT((a) * (1 << COORD_SHIFT))
#define FLOAT2COORD(a) (Coord)FLOAT2NUM((a))
#define COORD2FLOAT(a) ((a) / ((float)(1 << COORD_SHIFT)))
#define CLIPFLOAT(a) (((a) < 0) ? ((a) - 0.5) : ((a) + 0.5))

struct cc_oldcoord
{
	uint16_t x;
	uint16_t y;
};

/* Format of old calchart stuntsheet files */
#define OLD_FLAG_FLIP 1
struct cc_oldpoint
{
	uint8_t sym;
	uint8_t flags;
	cc_oldcoord pos;
	uint16_t color;
	int8_t code[2];
	uint16_t cont;
	cc_oldcoord ref[3];
};

struct cc_reallyoldpoint
{
	uint8_t sym;
	uint8_t flags;
	cc_oldcoord pos;
	uint16_t color;
	int8_t code[2];
	uint16_t cont;
	int16_t refnum;
	cc_oldcoord ref;
};

class CC_coord
{
public:
	CC_coord(Coord xval = 0, Coord yval = 0) : x(xval), y(yval) {}
	CC_coord(const cc_oldcoord& old) { *this = old; }

	float Magnitude() const;
	float DM_Magnitude() const;				  // check for diagonal military also
	float Direction() const;
	float Direction(const CC_coord& c) const;

	bool Collides(const CC_coord& c) const;

	CC_coord& operator = (const cc_oldcoord& old);
	inline CC_coord& operator += (const CC_coord& c)
	{
		x += c.x; y += c.y;
		return *this;
	}
	inline CC_coord& operator -= (const CC_coord& c)
	{
		x -= c.x; y -= c.y;
		return *this;
	}
	inline CC_coord& operator *= (short s)
	{
		x *= s; y *= s;
		return *this;
	}
	inline CC_coord& operator /= (short s)
	{
		x /= s; y /= s;
		return *this;
	}

	Coord x, y;
};
inline CC_coord operator + (const CC_coord& a, const CC_coord& b)
{
	return CC_coord(a.x + b.x, a.y + b.y);
}


inline CC_coord operator - (const CC_coord& a, const CC_coord& b)
{
	return CC_coord(a.x - b.x, a.y - b.y);
}


inline CC_coord operator * (const CC_coord& a, short s)
{
	return CC_coord(a.x * s, a.y * s);
}


inline CC_coord operator / (const CC_coord& a, short s)
{
	return CC_coord(a.x / s, a.y / s);
}


inline CC_coord operator - (const CC_coord& c)
{
	return CC_coord(-c.x, -c.y);
}


inline int operator == (const CC_coord& a, const CC_coord& b)
{
	return ((a.x == b.x) && (a.y == b.y));
}


inline int operator == (const CC_coord& a, const short b)
{
	return ((a.x == b) && (a.y == b));
}


inline int operator != (const CC_coord& a, const CC_coord& b)
{
	return ((a.x != b.x) || (a.y != b.y));
}


inline int operator != (const CC_coord& a, const short b)
{
	return ((a.x != b) || (a.y != b));
}


#endif