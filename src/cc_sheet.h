/* cc_sheet.h
 * Definitions for the sheet classes
 *
 */

/*
   Copyright (C) 1995-2010  Richard Powell

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

#ifdef __GNUG__
#pragma interface
#endif

#include "cc_types.h"
#include "cc_continuity.h"
#include "cc_point.h"

#include <wx/wx.h>
#include <vector>

class CC_show;
class CC_coord;
class Matrix;
class CC_textline;
struct cc_oldpoint;

typedef std::vector<CC_textline> CC_textline_list;

class CC_sheet
{
public:
	CC_sheet(CC_show *shw);
	CC_sheet(CC_show *shw, const wxString& newname);
	CC_sheet(CC_sheet *sht);
	~CC_sheet();

	unsigned GetNumSelectedPoints() const;
	int FindPoint(Coord x, Coord y, unsigned ref = 0) const;
	bool SelectContinuity(unsigned i) const;
	void SetContinuity(unsigned i);
	void SetNumPoints(unsigned num, unsigned columns);
	void RelabelSheet(unsigned *table);

	const CC_continuity_ptr GetNthContinuity(unsigned i) const;
	CC_continuity_ptr GetNthContinuity(unsigned i);
	const CC_continuity_ptr UserGetNthContinuity(unsigned i) const;
	CC_continuity_ptr UserGetNthContinuity(unsigned i);
	void SetNthContinuity(const wxString& text, unsigned i);
	void UserSetNthContinuity(const wxString& text, unsigned i, wxWindow* win);
	CC_continuity_ptr RemoveNthContinuity(unsigned i);
	void UserDeleteContinuity(unsigned i);
	void InsertContinuity(CC_continuity_ptr newcont, unsigned i);
	void AppendContinuity(CC_continuity_ptr newcont);
	CC_continuity_ptr UserNewContinuity(const wxString& name);
	unsigned NextUnusedContinuityNum();
// creates if doesn't exist
	CC_continuity_ptr GetStandardContinuity(SYMBOL_TYPE sym);
// return 0 if not found else index+1
	unsigned FindContinuityByName(const wxString& name) const;
	bool ContinuityInUse(unsigned idx) const;

	inline const wxString& GetName() const { return name; }
	inline void SetName(const wxString& newname) { name = newname; }
	inline const wxString& GetNumber() const { return number; }
	inline void SetNumber(const wxString& newnumber) { number = newnumber; }
	inline unsigned short GetBeats() const { return beats; }
	inline void SetBeats(unsigned short b) { beats = b; }
	inline bool IsInAnimation() const { return (beats != 0); }
	void UserSetName(const wxString& newname);
	void UserSetBeats(unsigned short b);
	bool SetPointsSym(SYMBOL_TYPE sym);
	bool SetPointsLabel(bool right);
	bool SetPointsLabelFlip();

	inline const CC_point& GetPoint(unsigned i) const { return pts[i]; }
	inline CC_point& GetPoint(unsigned i) { return pts[i]; }
	void SetPoint(const cc_oldpoint& val, unsigned i);

	const CC_coord& GetPosition(unsigned i, unsigned ref = 0) const;
	void SetAllPositions(const CC_coord& val, unsigned i);
	void SetPosition(const CC_coord& val, unsigned i, unsigned ref = 0);
	void SetPositionQuick(const CC_coord& val, unsigned i, unsigned ref = 0);
	bool ClearRefPositions(unsigned ref);
	bool TranslatePoints(const CC_coord& delta, unsigned ref = 0);
	bool TransformPoints(const Matrix& transmat, unsigned ref = 0);
	bool MovePointsInLine(const CC_coord& start, const CC_coord& second,
		unsigned ref);

	CC_sheet *next;
	CC_textline_list continuity;
	typedef std::vector<CC_continuity_ptr> ContContainer;
	ContContainer animcont;
	CC_show *show;
	bool picked;							  /* for requestors like printing */
private:
	unsigned short beats;
	std::vector<CC_point> pts;
	wxString name;
	wxString number;

friend void Draw(wxDC *dc, const CC_sheet& sheet, unsigned ref, bool primary, bool drawall, int point);
friend void DrawForPrinting(wxDC *dc, const CC_sheet& sheet, unsigned ref, bool landscape);
friend void DrawCont(wxDC& dc, const CC_sheet& sheet, wxCoord yStart, bool landscape);

friend const wxChar *PrintStandard(FILE *fp, const CC_sheet& sheet);
friend const wxChar *PrintSpringshow(FILE *fp, const CC_sheet& sheet);
friend const wxChar *PrintOverview(FILE *fp, const CC_sheet& sheet);
friend const wxChar *PrintCont(FILE *fp, const CC_sheet& sheet);

};

#endif