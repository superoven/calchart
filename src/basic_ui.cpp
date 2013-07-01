/* 
 * basic_ui.cpp
 * Handle basic wxWindows classes
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

#include "basic_ui.h"
#include "confgr.h"

#include <wx/dnd.h>
#include <wx/icon.h>

#include "calchart.xpm"

// Set icon to band's insignia
void SetBandIcon(wxFrame *frame)
{
	wxIcon icon = wxICON(calchart);
	frame->SetIcon(icon);
}


class FancyTextWinDropTarget : public wxFileDropTarget
{
public:
	FancyTextWinDropTarget(FancyTextWin *w) : win(w) {}
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
	{
		if (filenames.Count() > 0)
		{
			win->LoadFile(filenames[0]);
		}
		return true;
	}

private:
	FancyTextWin *win;
};


// Text subwindow that can respond to drag-and-drop
FancyTextWin::FancyTextWin(wxWindow* parent, wxWindowID id,
const wxString& value,
const wxPoint& pos,
const wxSize& size,
long style)
:wxTextCtrl(parent, id, value, pos, size, style)
{
	SetDropTarget(new FancyTextWinDropTarget(this));
}


#ifdef TEXT_DOS_STYLE
wxString FancyTextWin::GetValue(void) const
{
	wxString result;
	unsigned i;

// Remove carriage returns
	result = wxTextCtrl::GetValue();
	for (i = 0; i < result.length();)
	{
		if (result[i] != '\r') i++;
		else result.erase(i,1);
	}

	return result;
}
#endif


CtrlScrollCanvas::CtrlScrollCanvas(wxWindow *parent,
								   wxWindowID id,
								   const wxPoint& pos,
								   const wxSize& size,
								   long style) :
wxScrolledWindow(parent, id, pos, size, wxHSCROLL | wxVSCROLL | style),
mZoomFactor(1.0)
{
}


CtrlScrollCanvas::~CtrlScrollCanvas()
{
}


void
CtrlScrollCanvas::PrepareDC(wxDC& dc)
{
	super::PrepareDC(dc);
	dc.SetUserScale(mZoomFactor, mZoomFactor);
}


void
CtrlScrollCanvas::SetZoom(float z)
{
	mZoomFactor = z;
}

float
CtrlScrollCanvas::GetZoom() const
{
	return mZoomFactor;
}


void
CtrlScrollCanvas::OnMouseMove(wxMouseEvent &event)
{
	wxPoint thisPos = event.GetPosition();
	if (event.ControlDown())
	{
		mOffset += thisPos - mLastPos;
		Scroll(-mOffset);
		// cap the offset by how much the view moved
		wxPoint start = GetViewStart();
		mOffset = -start;
	}
	mLastPos = thisPos;
}

