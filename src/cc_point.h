/*
 * cc_point.h
 * Definitions for the point classes
 */

/*
   Copyright (C) 1995-2011  Garrick Brian Meeker, Richard Michael Powell

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

#ifndef _CC_POINT_H_
#define _CC_POINT_H_

#include "cc_types.h"
#include "cc_coord.h"

#include <bitset>

class wxDC;
class wxString;
class wxBrush;

// Perhaps this should be put in namespace?

class CC_point
{
public:
	static const unsigned kNumRefPoints = 3;
	CC_point();
	CC_point(uint8_t cont, const CC_coord& pos);

	bool GetFlip() const;
	void Flip(bool val = true);
	void FlipToggle();

	SYMBOL_TYPE GetSymbol() const;
	void SetSymbol(SYMBOL_TYPE sym);

	uint8_t GetContinuityIndex() const;
	void SetContinuityIndex(uint8_t cont);

	CC_coord GetPos() const;
	void SetPos(const CC_coord& c);

	// reference points are [1, kNumRefPoints]
	CC_coord GetRefPos(unsigned which) const;
	void SetRefPos(const CC_coord& c, unsigned which);

private:
	enum {
		kPointLabelFlipped,
		kTotalBits
	};
	
	std::bitset<kTotalBits> mFlags;
	// by having both a sym type and cont index, we can have several
	// points share the same symbol but have different continuities.
	SYMBOL_TYPE mSym;
	uint8_t mContinuityIndex;
	CC_coord mPos;
	CC_coord mRef[kNumRefPoints];

	friend struct CC_point_values;
	friend bool Check_CC_point(const CC_point&, const struct CC_point_values&);
};

// We break this out of the class to make CalChart internals more cross platform
// Draw the point
void DrawPoint(const CC_point& point, wxDC& dc, unsigned reference, const CC_coord& origin, const wxBrush *fillBrush, const wxString& label);

void CC_point_UnitTests();

#endif // _CC_POINT_H_
