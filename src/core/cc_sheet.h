/*
 * cc_sheet.h
 * Definitions for the calchart sheet classes
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

#ifndef _CC_SHEET_H_
#define _CC_SHEET_H_

#include "cc_types.h"
#include "cc_continuity.h"
#include "cc_point.h"
#include "cc_text.h"

#include <vector>
#include <set>
#include <string>

class CC_show;
class CC_coord;
class CC_continuity;
class CC_point;

// CalChart Sheet
// The CalChart sheet object is a collection of CC_point locations, the number of
// beats and the different marcher's continuity.

class CC_sheet
{
public:
	CC_sheet(CC_show *shw);
	CC_sheet(CC_show *shw, size_t numPoints, std::istream& stream, Version_3_3_and_earlier);
	CC_sheet(CC_show *shw, size_t numPoints, std::istream& stream, Current_version_and_later);
	CC_sheet(CC_show *shw, const std::string& newname);
	~CC_sheet();

private:
	std::vector<uint8_t> SerializeAllPoints() const;
	std::vector<uint8_t> SerializeContinuityData() const;
	std::vector<uint8_t> SerializePrintContinuityData() const;
	std::vector<uint8_t> SerializeSheetData() const;
public:
	std::vector<uint8_t> SerializeSheet() const;

	// Observer functions
	const CC_continuity& GetContinuityBySymbol(SYMBOL_TYPE i) const;
	std::set<unsigned> SelectPointsBySymbol(SYMBOL_TYPE i) const;
	bool ContinuityInUse(SYMBOL_TYPE idx) const;

	// setting values on the stunt sheet
	// * needs to be through command only *
	void SetNumPoints(unsigned num, unsigned columns, const CC_coord& new_march_position);

	// continuity:
	// * needs to be through command only *
	void SetContinuityText(SYMBOL_TYPE sym, const std::string& text);
	
	// points:
	int FindPoint(Coord x, Coord y, Coord searchBound, unsigned ref = 0) const;
	void RelabelSheet(const std::vector<size_t>& table);

	std::string GetName() const;
	void SetName(const std::string& newname);
	std::string GetNumber() const;
	void SetNumber(const std::string& newnumber);

	// beats
	unsigned short GetBeats() const;
	void SetBeats(unsigned short b);
	bool IsInAnimation() const { return (GetBeats() != 0); }

	const CC_point& GetPoint(unsigned i) const;
	CC_point& GetPoint(unsigned i);
	std::vector<CC_point> GetPoints() const;
	void SetPoints(const std::vector<CC_point>& points);

	CC_coord GetPosition(unsigned i, unsigned ref = 0) const;
	void SetAllPositions(const CC_coord& val, unsigned i);
	void SetPosition(const CC_coord& val, unsigned i, unsigned ref = 0);

	// continuity that gets printed
	void SetPrintableContinuity(const std::string& name, const std::string& lines);
	CC_textline_list GetPrintableContinuity() const;
	std::string GetRawPrintContinuity() const;

private:
	CC_continuity& GetContinuityBySymbol(SYMBOL_TYPE i);

	typedef std::vector<CC_continuity> ContContainer;
	ContContainer mAnimationContinuity;

	CC_print_continuity mPrintableContinuity;
	unsigned short beats;
	std::vector<CC_point> pts;
	std::string mName;
};

#endif
