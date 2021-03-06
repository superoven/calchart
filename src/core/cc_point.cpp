/*
 * cc_point.cpp
 * Definition for the point classes
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

#include "cc_point.h"
#include "cc_fileformat.h"

#include <assert.h>
#include <stdexcept>
#include <sstream>

CC_point::CC_point() :
mSym(SYMBOL_PLAIN)
{
}

CC_point::CC_point(const CC_coord& p) :
mSym(SYMBOL_PLAIN),
mPos(p)
{
	for (unsigned j = 0; j < CC_point::kNumRefPoints; j++)
	{
		mRef[j] = mPos;
	}
}

// EACH_POINT_DATA    = BigEndianInt8(Size_rest_of_EACH_POINT_DATA) , POSITION_DATA , REF_POSITION_DATA , POINT_SYMBOL_DATA , POINT_LABEL_FLIP ;
// POSITION_DATA      = BigEndianInt16( x ) , BigEndianInt16( y ) ;
// REF_POSITION_DATA  = BigEndianInt8( num ref pts ) , { BigEndianInt8( which reference point ) , BigEndianInt16( x ) , BigEndianInt16( y ) }* ;
// POINT_SYMBOL_DATA  = BigEndianInt8( which symbol type ) ;
// POINT_LABEL_FLIP_DATA = BigEndianInt8( label flipped ) ;
CC_point::CC_point(const std::vector<uint8_t>& serialized_data) :
mSym(SYMBOL_PLAIN)
{
	const uint8_t *d = &serialized_data[0];
	{
		mPos.x = get_big_word(d);
		d += 2;
		mPos.y = get_big_word(d);
		d += 2;
	}
	// set all the reference points
	for (unsigned j = 0; j < CC_point::kNumRefPoints; j++)
	{
		mRef[j] = mPos;
	}
	
	uint8_t num_ref = *d;
	++d;
	for (auto i = 0; i < num_ref; ++i)
	{
		uint8_t ref = *d;
		++d;
		mRef[ref-1].x = get_big_word(d);
		d += 2;
		mRef[ref-1].y = get_big_word(d);
		d += 2;
	}
	mSym = static_cast<SYMBOL_TYPE>(*d);
	++d;
	mFlags.set(kPointLabelFlipped, *d);
	++d;
	if (std::distance(&serialized_data[0], d) != serialized_data.size())
	{
		throw CC_FileException("bad POS chunk");
	}
}

std::vector<uint8_t>
CC_point::Serialize() const
{
	// how many reference points are we going to write?
	uint8_t num_ref_pts = 0;
	for (auto j = 1; j <= CC_point::kNumRefPoints; j++)
	{
		if (GetPos(j) != GetPos(0))
		{
			++num_ref_pts;
		}
	}
	uint8_t size_of_each_point_data = (num_ref_pts * 5) + 2 + 2 + 1 + 1 + 1;

	// Point positions
	std::ostringstream stream("");

	// Write block size
	Write(stream, &size_of_each_point_data, sizeof(size_of_each_point_data));

	// Write POSITION
	Coord crd;
	put_big_word(&crd, GetPos().x);
	Write(stream, &crd, 2);
	put_big_word(&crd, GetPos().y);
	Write(stream, &crd, 2);

	// Write REF_POS
	Write(stream, &num_ref_pts, 1);
	if (num_ref_pts)
	{
		for (auto j = 1; j <= CC_point::kNumRefPoints; j++)
		{
			if (GetPos(j) != GetPos(0))
			{
				uint8_t which = j;
				Write(stream, &which, 1);
				Coord crd;
				put_big_word(&crd, GetPos(j).x);
				Write(stream, &crd, 2);
				put_big_word(&crd, GetPos(j).y);
				Write(stream, &crd, 2);
			}
		}
	}

	// Write POSITION
	uint8_t tmp = GetSymbol();
	Write(stream, &tmp, 1);

	// Point labels (left or right)
	tmp = (GetFlip()) ? true : false;
	Write(stream, &tmp, 1);

	auto sdata = stream.str();
	std::vector<uint8_t> data;
	std::copy(sdata.begin(), sdata.end(), std::back_inserter(data));
	return data;
}

bool
CC_point::GetFlip() const
{
	return mFlags.test(kPointLabelFlipped);
}

void
CC_point::Flip(bool val)
{
	mFlags.set(kPointLabelFlipped, val);
};

void
CC_point::FlipToggle()
{
	mFlags.flip(kPointLabelFlipped);
}

CC_coord
CC_point::GetPos(unsigned ref) const
{
	if (ref == 0)
	{
		return mPos;
	}
	if (ref > kNumRefPoints)
	{
		throw std::range_error("GetPos() point out of range");
	}
	return mRef[ref-1];
}

void
CC_point::SetPos(const CC_coord& c, unsigned ref)
{
	if (ref == 0)
	{
		mPos = c;
		return;
	}
	if (ref > kNumRefPoints)
	{
		throw std::range_error("SetRefPos() point out of range");
	}
	mRef[ref-1] = c;
}

SYMBOL_TYPE
CC_point::GetSymbol() const
{
	return mSym;
}

void
CC_point::SetSymbol(SYMBOL_TYPE s)
{
	mSym = s;
}

// Test Suite stuff
struct CC_point_values
{
	std::bitset<CC_point::kTotalBits> mFlags;
	SYMBOL_TYPE mSym;
	CC_coord mPos;
	CC_coord mRef[CC_point::kNumRefPoints];
	bool GetFlip;
};

bool Check_CC_point(const CC_point& underTest, const CC_point_values& values)
{
	bool running_value = true;
	for (unsigned i = 0; i < CC_point::kNumRefPoints; ++i)
		running_value = running_value && (underTest.mRef[i] == values.mRef[i]);
	return running_value
		&& (underTest.mFlags == values.mFlags)
		&& (underTest.mSym == values.mSym)
		&& (underTest.mPos == values.mPos)
		&& (underTest.GetFlip() == values.GetFlip)
		;
}

void CC_point_UnitTests()
{
	// test some defaults:
	CC_point_values values;
	values.mFlags = 0;
	values.mSym = SYMBOL_PLAIN;
	values.mPos = CC_coord();
	for (unsigned i = 0; i < CC_point::kNumRefPoints; ++i)
		values.mRef[i] = CC_coord();
	values.GetFlip = false;

	// test defaults
	CC_point underTest;
	assert(Check_CC_point(underTest, values));

	// test flip
	underTest.Flip(false);
	assert(Check_CC_point(underTest, values));

	values.mFlags = 1;
	values.GetFlip = true;
	underTest.Flip(true);
	assert(Check_CC_point(underTest, values));

	values.mFlags = 0;
	values.GetFlip = false;
	underTest.Flip(false);
	assert(Check_CC_point(underTest, values));

	// test flip toggle
	values.mFlags = 1;
	values.GetFlip = true;
	underTest.FlipToggle();
	assert(Check_CC_point(underTest, values));

	values.mFlags = 0;
	values.GetFlip = false;
	underTest.FlipToggle();
	assert(Check_CC_point(underTest, values));

}
