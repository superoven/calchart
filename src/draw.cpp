/* draw.cpp
 * Member functions for drawing stuntsheets
 *
 * Modification history:
 * 6-22-95    Garrick Meeker              Created
 *
 */

/*
   Copyright (C) 1995-2008  Garrick Brian Meeker

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

#include <wx/dc.h>
#include <wx/dcmemory.h>
#include "show.h"
#include "confgr.h"
#include "modes.h"
#include "cc_sheet.h"
#include <memory>

extern wxFont *pointLabelFont;

void CC_sheet::Draw(wxDC *dc, unsigned ref, bool primary,
bool drawall, int point)
{
	unsigned short i;
	unsigned firstpoint, lastpoint;
	const wxBrush *fillBrush;
	unsigned long x, y;
	float offset, plineoff, slineoff, textoff;
	wxCoord textw, texth, textd;
	float circ_r;
	CC_coord origin;

	dc->SetBackgroundMode(wxTRANSPARENT);
	dc->SetBackground(*CalChartBrushes[COLOR_FIELD]);

	if (drawall)
	{
		dc->Clear();
		dc->SetPen(*CalChartPens[COLOR_FIELD_DETAIL]);
		dc->SetTextForeground(CalChartPens[COLOR_FIELD_TEXT]->GetColour());
		dc->SetLogicalFunction(wxCOPY);
		show->GetMode().Draw(dc);
	}

	if (!pts.empty())
	{
		dc->SetFont(*pointLabelFont);
		dc->SetTextForeground(CalChartPens[COLOR_POINT_TEXT]->GetColour());
		circ_r = FLOAT2COORD(dot_ratio);
		offset = circ_r / 2;
		plineoff = offset * pline_ratio;
		slineoff = offset * sline_ratio;
		textoff = offset * 1.25;
		origin = show->GetMode().Offset();
		if (point < 0)
		{
			firstpoint = 0;
			lastpoint = show->GetNumPoints();
		}
		else
		{
			firstpoint = (unsigned)point;
			lastpoint = (unsigned)point + 1;
		}
		for (int selectd = 0; selectd < 2; selectd++)
		{
			for (i = firstpoint; i < lastpoint; i++)
			{
				if ((show->IsSelected(i) != 0) == (selectd != 0))
				{
					if (selectd)
					{
						if (primary)
						{
							dc->SetPen(*CalChartPens[COLOR_POINT_HILIT]);
							fillBrush = CalChartBrushes[COLOR_POINT_HILIT];
						}
						else
						{
							dc->SetPen(*CalChartPens[COLOR_REF_POINT_HILIT]);
							fillBrush = CalChartBrushes[COLOR_REF_POINT_HILIT];
						}
					}
					else
					{
						if (primary)
						{
							dc->SetPen(*CalChartPens[COLOR_POINT]);
							fillBrush = CalChartBrushes[COLOR_POINT];
						}
						else
						{
							dc->SetPen(*CalChartPens[COLOR_REF_POINT]);
							fillBrush = CalChartBrushes[COLOR_REF_POINT];
						}
					}
					x = GetPosition(i, ref).x+origin.x;
					y = GetPosition(i, ref).y+origin.y;
					switch (pts[i].sym)
					{
						case SYMBOL_SOL:
						case SYMBOL_SOLBKSL:
						case SYMBOL_SOLSL:
						case SYMBOL_SOLX:
							dc->SetBrush(*fillBrush);
							break;
						default:
							dc->SetBrush(*wxTRANSPARENT_BRUSH);
					}
					dc->DrawEllipse(x - offset, y - offset, circ_r, circ_r);
					switch (pts[i].sym)
					{
						case SYMBOL_SL:
						case SYMBOL_X:
							dc->DrawLine(x - plineoff, y + plineoff,
								x + plineoff, y - plineoff);
							break;
						case SYMBOL_SOLSL:
						case SYMBOL_SOLX:
							dc->DrawLine(x - slineoff, y + slineoff,
								x + slineoff, y - slineoff);
							break;
						default:
							break;
					}
					switch (pts[i].sym)
					{
						case SYMBOL_BKSL:
						case SYMBOL_X:
							dc->DrawLine(x - plineoff, y - plineoff,
								x + plineoff, y + plineoff);
							break;
						case SYMBOL_SOLBKSL:
						case SYMBOL_SOLX:
							dc->DrawLine(x - slineoff, y - slineoff,
								x + slineoff, y + slineoff);
							break;
						default:
							break;
					}
					dc->GetTextExtent(show->GetPointLabel(i), &textw, &texth, &textd);
					dc->DrawText(show->GetPointLabel(i),
						pts[i].GetFlip() ? x : (x - textw),
						y - textoff - texth + textd);
				}
			}
		}
	}
	dc->SetFont(wxNullFont);
}

// draw text centered around x (though still at y down)
void DrawCenteredText(wxDC& dc, const wxString& text, const wxPoint& pt)
{
	wxCoord w, h;
	dc.GetTextExtent(text, &w, &h);
	w = std::max(0, pt.x - w/2);
	dc.DrawText(text, w, pt.y);
}

// draw text centered around x (though still at y down) with a line over it.
void DrawLineOverText(wxDC& dc, const wxString& text, const wxPoint& pt, wxCoord lineLength)
{
	DrawCenteredText(dc, text, pt + wxPoint(0,2));
	dc.DrawLine(pt - wxPoint(lineLength/2,0), pt + wxPoint(lineLength/2,0));
}

static const size_t ArrowSize = 4;
void DrawArrow(wxDC& dc, const wxPoint& pt, wxCoord lineLength, bool pointRight)
{
	dc.DrawLine(pt + wxPoint(-lineLength/2,ArrowSize), pt + wxPoint(lineLength/2,ArrowSize));
	if (pointRight)
	{
		dc.DrawLine(pt + wxPoint(lineLength/2 - ArrowSize,0), pt + wxPoint(lineLength/2,ArrowSize));
		dc.DrawLine(pt + wxPoint(lineLength/2 - ArrowSize,ArrowSize*2), pt + wxPoint(lineLength/2,ArrowSize));
	}
	else
	{
		dc.DrawLine(pt + wxPoint(-(lineLength/2 - ArrowSize),0), pt + wxPoint(-lineLength/2,ArrowSize));
		dc.DrawLine(pt + wxPoint(-(lineLength/2 - ArrowSize),ArrowSize*2), pt + wxPoint(-lineLength/2,ArrowSize));
	}
}

// calculate the distance for tab stops
size_t TabStops(size_t which, bool landscape)
{
	size_t tab = 0;
	while (which > 4)
	{
		which--;
		tab += (landscape)?8:6;
	}
	switch (which)
	{
		case 3:
			tab += 8;
		case 2:
			tab += (landscape)?18:14;
		case 1:
			tab += (landscape)?10:6;
	}
	return tab;
}

// these are the sizes that the page is set up to do.
static const double kBitmapScale = 2.0; // the factor to scale the bitmap
static const double kContinuityScale = 2.0; // the factor to scale the continuity
static const double kSizeX = 576, kSizeY = 734;
static const double kSizeXLandscape = 917, kSizeYLandscape = 720;
static const double kHeaderLocation[2][2] = { { 0.5, 18/kSizeY }, { 0.5, 22/kSizeYLandscape } };
static const wxString kHeader = wxT("UNIVERSITY OF CALIFORNIA MARCHING BAND");
static const double kUpperNumberPosition[2][2] = { { 1.0 - 62/kSizeX, 36/kSizeY }, { 1.0 - 96/kSizeXLandscape, 36/kSizeYLandscape } };
static const double kLowerNumberPosition[2][2] = { { 1.0 - 62/kSizeX, 714/kSizeY }, { 1.0 - 96/kSizeXLandscape, 680/kSizeYLandscape } };
static const double kLowerNumberBox[2][4] = { { 1.0 - 90/kSizeX, 708/kSizeY, 56/kSizeX, 22/kSizeY }, { 1.0 - 124/kSizeXLandscape, 674/kSizeYLandscape, 56/kSizeXLandscape, 28/kSizeYLandscape } };
static const double kMusicLabelPosition[2][3] = { { 0.5, 60/kSizeY, 240/kSizeX }, { 0.5, 60/kSizeYLandscape, 400/kSizeXLandscape } };
static const wxString kMusicLabel = wxT("Music");
static const double kFormationLabelPosition[2][3] = { { 0.5, 82/kSizeY, 240/kSizeX }, { 0.5, 82/kSizeYLandscape, 400/kSizeXLandscape } };
static const wxString kFormationLabel = wxT("Formation");

static const double kGameLabelPosition[2][3] = { { 62/kSizeX, 50/kSizeY, 64/kSizeX }, { 96/kSizeXLandscape, 54/kSizeYLandscape, 78/kSizeXLandscape } };
static const wxString kGameLabel = wxT("game");
static const double kPageLabelPosition[2][3] = { { 1.0 - 62/kSizeX, 50/kSizeY, 64/kSizeX }, { 1.0 - 96/kSizeXLandscape, 54/kSizeYLandscape, 78/kSizeXLandscape } };
static const wxString kPageLabel = wxT("page");
static const double kSideLabelPosition[2][2] = { { 0.5, 580/kSizeY }, { 0.5, 544/kSizeYLandscape } };
static const wxString kSideLabel = wxT("CAL SIDE");

static const double kUpperSouthPosition[2][2] = { { 52/kSizeX, (76-8)/kSizeY }, { 76/kSizeXLandscape, (80-8)/kSizeYLandscape } };
static const wxString kUpperSouthLabel = wxT("south");
static const double kUpperSouthArrow[2][3] = { { 52/kSizeX, (76)/kSizeY, 40/kSizeX }, { 76/kSizeXLandscape, (80)/kSizeYLandscape, 40/kSizeXLandscape } };
static const double kUpperNorthPosition[2][2] = { { 1.0 - 52/kSizeX, (76-8)/kSizeY }, { 1.0 - 76/kSizeXLandscape, (80-8)/kSizeYLandscape } };
static const wxString kUpperNorthLabel = wxT("north");
static const double kUpperNorthArrow[2][3] = { { 1.0 - 52/kSizeX, (76)/kSizeY, 40/kSizeX }, { 1.0 - 76/kSizeXLandscape, (80)/kSizeYLandscape, 40/kSizeXLandscape } };

static const double kLowerSouthPosition[2][2] = { { 52/kSizeX, (570+8)/kSizeY }, { 76/kSizeXLandscape, (536+8)/kSizeYLandscape } };
static const wxString kLowerSouthLabel = wxT("south");
static const double kLowerSouthArrow[2][3] = { { 52/kSizeX, (570)/kSizeY, 40/kSizeX }, { 76/kSizeXLandscape, (536)/kSizeYLandscape, 40/kSizeXLandscape } };
static const double kLowerNorthPosition[2][2] = { { 1.0 - 52/kSizeX, (570+8)/kSizeY }, { 1.0 - 76/kSizeXLandscape, (536+8)/kSizeYLandscape } };
static const wxString kLowerNorthLabel = wxT("north");
static const double kLowerNorthArrow[2][3] = { { 1.0 - 52/kSizeX, (570)/kSizeY, 40/kSizeX }, { 1.0 - 76/kSizeXLandscape, (536)/kSizeYLandscape, 40/kSizeXLandscape } };

static const double kContinuityStart[2] = { 606/kSizeY, 556/kSizeYLandscape };

// draw the continuity starting at a specific offset
void CC_sheet::DrawCont(wxDC& dc, const wxCoord yStart, bool landscape) const
{
	float x, y;
	wxCoord textw, texth, textd, maxtexth;

	wxCoord origX, origY;
	double origXscale, origYscale;

	dc.GetDeviceOrigin(&origX, &origY);
	dc.GetUserScale(&origXscale, &origYscale);
	const wxColour& origForegroundColor = dc.GetTextForeground();
	const wxFont& origFont = dc.GetFont();

	// we set the font large then scale down to give fine detail on the page
	wxFont *contPlainFont = wxTheFontList->FindOrCreateFont(20, wxMODERN, wxNORMAL, wxNORMAL);
	wxFont *contBoldFont = wxTheFontList->FindOrCreateFont(20, wxMODERN, wxNORMAL, wxBOLD);
	wxFont *contItalFont = wxTheFontList->FindOrCreateFont(20, wxMODERN, wxITALIC, wxNORMAL);
	wxFont *contBoldItalFont = wxTheFontList->FindOrCreateFont(20, wxMODERN, wxITALIC, wxBOLD);

	dc.SetDeviceOrigin(20, yStart);
	dc.SetTextForeground(*wxBLACK);
	dc.SetFont(*contPlainFont);
	
	int pageW, pageH;
	dc.GetSize(&pageW, &pageH);
	int pageMiddle;
	if (landscape)
	{
		dc.SetUserScale((pageW/kSizeXLandscape)/kContinuityScale, (pageH/kSizeYLandscape)/kContinuityScale);
		pageMiddle = (pageW * (1.0/((pageW/kSizeXLandscape)/kContinuityScale)))/2 - 20;
	}
	else
	{
		dc.SetUserScale((pageW/kSizeX)/kContinuityScale, (pageH/kSizeY)/kContinuityScale);
		pageMiddle = (pageW * (1.0/((pageW/kSizeX)/kContinuityScale)))/2 - 20;
	}
	y = 0.0;
	const wxCoord charWidth = dc.GetCharWidth();
	CC_textline_list::const_iterator cont(continuity.begin());
	while (cont != continuity.end())
	{
		bool do_tab;
		CC_textchunk_list::const_iterator c;
		x = 0.0;
		if (cont->center)
		{
			x += pageMiddle;
			for (c = cont->chunks.begin();
				c != cont->chunks.end();
				++c)
			{
				do_tab = false;
				switch (c->font)
				{
					case PSFONT_SYMBOL:
						dc.GetTextExtent(wxT("O"), &textw, &texth, &textd);
						x += textw * c->text.length();
						break;
					case PSFONT_NORM:
						dc.SetFont(*contPlainFont);
						break;
					case PSFONT_BOLD:
						dc.SetFont(*contBoldFont);
						break;
					case PSFONT_ITAL:
						dc.SetFont(*contItalFont);
						break;
					case PSFONT_BOLDITAL:
						dc.SetFont(*contBoldItalFont);
						break;
					case PSFONT_TAB:
						do_tab = true;
						break;
				}
				if (!do_tab && (c->font != PSFONT_SYMBOL))
				{
					dc.GetTextExtent(c->text, &textw, &texth, &textd);
					x -= textw/2;
				}
			}
		}
		maxtexth = contPlainFont->GetPointSize()+2;
		unsigned tabnum = 0;
		for (c = cont->chunks.begin();
			c != cont->chunks.end();
			++c)
		{
			do_tab = false;
			switch (c->font)
			{
				case PSFONT_NORM:
				case PSFONT_SYMBOL:
					dc.SetFont(*contPlainFont);
					break;
				case PSFONT_BOLD:
					dc.SetFont(*contBoldFont);
					break;
				case PSFONT_ITAL:
					dc.SetFont(*contItalFont);
					break;
				case PSFONT_BOLDITAL:
					dc.SetFont(*contBoldItalFont);
					break;
				case PSFONT_TAB:
					tabnum++;
					textw = charWidth * TabStops(tabnum, landscape);
					if (textw >= x) x = textw;
					else x += charWidth;
					do_tab = true;
					break;
				default:
					break;
			}
			if (c->font == PSFONT_SYMBOL)
			{
				dc.GetTextExtent(wxT("O"), &textw, &texth, &textd);
				float d = textw;
				SYMBOL_TYPE sym;

				float top_y = y + texth - textd - d;

				for (const wxChar *s = c->text; *s; s++)
				{
					{
						dc.SetPen(*wxBLACK_PEN);
						sym = (SYMBOL_TYPE)(*s - 'A');
						switch (sym)
						{
							case SYMBOL_SOL:
							case SYMBOL_SOLBKSL:
							case SYMBOL_SOLSL:
							case SYMBOL_SOLX:
								dc.SetBrush(*wxBLACK_BRUSH);
								break;
							default:
								dc.SetBrush(*wxTRANSPARENT_BRUSH);
						}
						dc.DrawEllipse(x, top_y, d, d);
						switch (sym)
						{
							case SYMBOL_SL:
							case SYMBOL_X:
							case SYMBOL_SOLSL:
							case SYMBOL_SOLX:
								dc.DrawLine(x-1, top_y + d+1, x + d+1, top_y-1);
								break;
							default:
								break;
						}
						switch (sym)
						{
							case SYMBOL_BKSL:
							case SYMBOL_X:
							case SYMBOL_SOLBKSL:
							case SYMBOL_SOLX:
								dc.DrawLine(x-1, top_y-1, x + d+1, top_y + d+1);
								break;
							default:
								break;
						}
					}
					x += d;
				}
			}
			else
			{
				if (!do_tab)
				{
					dc.GetTextExtent(c->text, &textw, &texth, &textd);
					dc.DrawText(c->text, x, y);
					x += textw;
				}
			}
		}
		y += maxtexth;
		++cont;
	}

	// restore everything
	dc.SetUserScale(origXscale, origYscale);
	dc.SetDeviceOrigin(origX, origY);
	dc.SetTextForeground(origForegroundColor);
	dc.SetFont(origFont);
}

static std::auto_ptr<ShowMode> CreateFieldForPrinting(bool landscape)
{
	CC_coord bord1(INT2COORD(8),INT2COORD(8)), bord2(INT2COORD(8),INT2COORD(8));
	CC_coord siz, off;
	uint32_t whash = 32;
	uint32_t ehash = 52;
	bord1.x = INT2COORD((landscape)?12:6);
	bord1.y = INT2COORD((landscape)?21:19);
	bord2.x = INT2COORD((landscape)?12:6);
	bord2.y = INT2COORD((landscape)?12:6);
	siz.x = INT2COORD((landscape)?160:96);
	siz.y = INT2COORD(84);
	off.x = INT2COORD((landscape)?80:48);
	off.y = INT2COORD(42);

	return std::auto_ptr<ShowMode>(new ShowModeStandard(wxT("Standard"), bord1, bord2, siz, off, whash, ehash));
}

void CC_sheet::DrawForPrinting(wxDC *printerdc, unsigned ref, bool landscape) const
{
	unsigned short i;
	unsigned long x, y;
	float circ_r;
	CC_coord origin;

	std::auto_ptr<wxBitmap> membm(new wxBitmap((landscape?kSizeXLandscape:kSizeX)*kBitmapScale, (landscape?kSizeYLandscape:kSizeY)*kBitmapScale));
	std::auto_ptr<wxDC> dc(new wxMemoryDC(*membm));
	wxCoord origX, origY;
	double origXscale, origYscale;
	dc->GetDeviceOrigin(&origX, &origY);
	dc->GetUserScale(&origXscale, &origYscale);

	if (landscape)
	{
		dc->SetDeviceOrigin(10, 0);
	}

	// create a field for drawing:
	std::auto_ptr<ShowMode> mode = CreateFieldForPrinting(landscape);

	int pageW, pageH;
	dc->GetSize(&pageW, &pageH);

	// set the scaling for drawing the field
	dc->SetUserScale(pageW/(double)mode->Size().x, pageW/(double)mode->Size().x);

	// draw the field.
	dc->Clear();
	dc->SetPen(*wxBLACK_PEN);
	dc->SetTextForeground(*wxBLACK);
	dc->SetLogicalFunction(wxCOPY);
	mode->Draw(dc.get());

	if (!pts.empty())
	{
		dc->SetFont(*pointLabelFont);
		circ_r = FLOAT2COORD(dot_ratio);
		const float offset = circ_r / 2;
		const float plineoff = offset * pline_ratio;
		const float slineoff = offset * sline_ratio;
		const float textoff = offset * 1.25;
		origin = mode->Offset();
		for (int selectd = 0; selectd < 2; selectd++)
		{
			for (i = 0; i < show->GetNumPoints(); i++)
			{
				x = GetPosition(i, ref).x+origin.x;
				y = GetPosition(i, ref).y+origin.y;
				switch (pts[i].sym)
				{
					case SYMBOL_SOL:
					case SYMBOL_SOLBKSL:
					case SYMBOL_SOLSL:
					case SYMBOL_SOLX:
						dc->SetBrush(*wxBLACK_BRUSH);
						break;
					default:
						dc->SetBrush(*wxTRANSPARENT_BRUSH);
				}
				dc->DrawEllipse(x - offset, y - offset, circ_r, circ_r);
				switch (pts[i].sym)
				{
					case SYMBOL_SL:
					case SYMBOL_X:
						dc->DrawLine(x - plineoff, y + plineoff,
							x + plineoff, y - plineoff);
						break;
					case SYMBOL_SOLSL:
					case SYMBOL_SOLX:
						dc->DrawLine(x - slineoff, y + slineoff,
							x + slineoff, y - slineoff);
						break;
					default:
						break;
				}
				switch (pts[i].sym)
				{
					case SYMBOL_BKSL:
					case SYMBOL_X:
						dc->DrawLine(x - plineoff, y - plineoff,
							x + plineoff, y + plineoff);
						break;
					case SYMBOL_SOLBKSL:
					case SYMBOL_SOLX:
						dc->DrawLine(x - slineoff, y - slineoff,
							x + slineoff, y + slineoff);
						break;
					default:
						break;
				}
				wxCoord textw, texth, textd;
				dc->GetTextExtent(show->GetPointLabel(i), &textw, &texth, &textd);
				dc->DrawText(show->GetPointLabel(i),
					pts[i].GetFlip() ? x : (x - textw),
					y - textoff - texth + textd);
			}
		}
	}

	dc->SetBrush(*wxTRANSPARENT_BRUSH);
	// set the page for drawing:
	dc->GetSize(&pageW, &pageH);
	if (landscape)
	{
		dc->SetUserScale(pageW/kSizeXLandscape, pageH/kSizeYLandscape);
		pageW = kSizeXLandscape;
		pageH = kSizeYLandscape;
	}
	else
	{
		dc->SetUserScale(pageW/kSizeX, pageH/kSizeY);
		pageW = kSizeX;
		pageH = kSizeY;
	}

	// draw the header
	dc->SetFont(*wxTheFontList->FindOrCreateFont(16, wxROMAN, wxNORMAL, wxBOLD));
	dc->SetPen(*wxThePenList->FindOrCreatePen(*wxBLACK, 1, wxSOLID));

	DrawCenteredText(*dc, kHeader, wxPoint(pageW*kHeaderLocation[landscape][0], pageH*kHeaderLocation[landscape][1]));

	DrawCenteredText(*dc, GetNumber(), wxPoint(pageW*kUpperNumberPosition[landscape][0], pageH*kUpperNumberPosition[landscape][1]));
	DrawCenteredText(*dc, GetNumber(), wxPoint(pageW*kLowerNumberPosition[landscape][0], pageH*kLowerNumberPosition[landscape][1]));
	dc->DrawRectangle(pageW*kLowerNumberBox[landscape][0], pageH*kLowerNumberBox[landscape][1], pageW*kLowerNumberBox[landscape][2], pageH*kLowerNumberBox[landscape][3]);

	dc->SetFont(*wxTheFontList->FindOrCreateFont(8, wxSWISS, wxNORMAL, wxNORMAL));

	DrawLineOverText(*dc, kMusicLabel, wxPoint(pageW*kMusicLabelPosition[landscape][0], pageH*kMusicLabelPosition[landscape][1]), pageW*kMusicLabelPosition[landscape][2]);
	DrawLineOverText(*dc, kFormationLabel, wxPoint(pageW*kFormationLabelPosition[landscape][0], pageH*kFormationLabelPosition[landscape][1]), pageW*kFormationLabelPosition[landscape][2]);
	DrawLineOverText(*dc, kGameLabel, wxPoint(pageW*kGameLabelPosition[landscape][0], pageH*kGameLabelPosition[landscape][1]), pageW*kGameLabelPosition[landscape][2]);
	DrawLineOverText(*dc, kPageLabel, wxPoint(pageW*kPageLabelPosition[landscape][0], pageH*kPageLabelPosition[landscape][1]), pageW*kPageLabelPosition[landscape][2]);
	DrawCenteredText(*dc, kSideLabel, wxPoint(pageW*kSideLabelPosition[landscape][0], pageH*kSideLabelPosition[landscape][1]));

	// draw arrows
	DrawCenteredText(*dc, kUpperSouthLabel, wxPoint(pageW*kUpperSouthPosition[landscape][0], pageH*kUpperSouthPosition[landscape][1]));
	DrawArrow(*dc, wxPoint(pageW*kUpperSouthArrow[landscape][0], pageH*kUpperSouthArrow[landscape][1]), pageW*kUpperSouthArrow[landscape][2], false);
	DrawCenteredText(*dc, kUpperNorthLabel, wxPoint(pageW*kUpperNorthPosition[landscape][0], pageH*kUpperNorthPosition[landscape][1]));
	DrawArrow(*dc, wxPoint(pageW*kUpperNorthArrow[landscape][0], pageH*kUpperNorthArrow[landscape][1]), pageW*kUpperNorthArrow[landscape][2], true);
	DrawCenteredText(*dc, kLowerSouthLabel, wxPoint(pageW*kLowerSouthPosition[landscape][0], pageH*kLowerSouthPosition[landscape][1]));
	DrawArrow(*dc, wxPoint(pageW*kLowerSouthArrow[landscape][0], pageH*kLowerSouthArrow[landscape][1]), pageW*kLowerSouthArrow[landscape][2], false);
	DrawCenteredText(*dc, kLowerNorthLabel, wxPoint(pageW*kLowerNorthPosition[landscape][0], pageH*kLowerNorthPosition[landscape][1]));
	DrawArrow(*dc, wxPoint(pageW*kLowerNorthArrow[landscape][0], pageH*kLowerNorthArrow[landscape][1]), pageW*kLowerNorthArrow[landscape][2], true);

	DrawCont(*dc, kBitmapScale*pageH*kContinuityStart[landscape], landscape);

	dc->SetUserScale(origXscale, origYscale);
	dc->SetDeviceOrigin(origX, origY);

	dc->SetFont(wxNullFont);
	
	int printerW, printerH;
	printerdc->GetSize(&printerW, &printerH);
	printerdc->SetUserScale(printerW/((landscape?kSizeXLandscape:kSizeX)*kBitmapScale), printerH/((landscape?kSizeYLandscape:kSizeY)*kBitmapScale));
	printerdc->Blit(0, 0, membm->GetWidth(), membm->GetHeight(), dc.get(), 0, 0);
}
