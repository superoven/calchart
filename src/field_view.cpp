/*
 * field_view.cpp
 * Header for field view
 */

/*
   Copyright (C) 1995-2012  Garrick Brian Meeker, Richard Michael Powell

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

#include "field_view.h"
#include "field_canvas.h"

#include "calchartapp.h"
#include "confgr.h"
#include "field_frame.h"
#include "top_frame.h"
#include "animate.h"
#include "show_ui.h"
#include "cc_command.h"
#include "cc_shapes.h"
#include "setup_wizards.h"

#include <wx/wizard.h>

IMPLEMENT_DYNAMIC_CLASS(FieldView, wxView)

FieldView::FieldView() :
mFrame(NULL),
mDrawPaths(false),
mCurrentReferencePoint(0)
{
}

FieldView::~FieldView()
{
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool
FieldView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
	mShow = static_cast<CC_show*>(doc);
	mFrame = new FieldFrame(doc, this, wxStaticCast(wxGetApp().GetTopWindow(), wxDocMDIParentFrame), wxPoint(50, 50),
							wxSize(GetConfiguration_FieldFrameWidth(), GetConfiguration_FieldFrameHeight()));
	
	mFrame->Show(true);
	Activate(true);
	return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void
FieldView::OnDraw(wxDC *dc)
{
	if (mShow)
	{
		// draw the field
		dc->SetPen(*CalChartPens[COLOR_FIELD_DETAIL]);
		dc->SetTextForeground(CalChartPens[COLOR_FIELD_TEXT]->GetColour());
		mShow->GetMode().Draw(*dc);
		
		CC_show::const_CC_sheet_iterator_t sheet = mShow->GetCurrentSheet();
		if (sheet != mShow->GetSheetEnd())
		{
			if (mCurrentReferencePoint > 0)
			{
				mShow->DrawShow(*dc, 0, false);
				mShow->DrawShow(*dc, mCurrentReferencePoint, true);
			}
			else
			{
				mShow->DrawShow(*dc, mCurrentReferencePoint, true);
			}
			DrawPaths(*dc, *sheet);
		}
	}
}

void
FieldView::OnUpdate(wxView *WXUNUSED(sender), wxObject *hint)
{
	if (hint && hint->IsKindOf(CLASSINFO(CC_show_setup)))
	{
		// give our show a first page
		CC_show* show = static_cast<CC_show*>(GetDocument());
		show->InsertSheetInternal(CC_sheet(show, wxT("1")), 0);
		show->SetCurrentSheet(0);
		
		// Set up everything else
		OnWizardSetup(*show);
		
		// make the show modified so it gets repainted
		show->Modify(true);
	}
	else
	{
		if (mFrame)
		{
			mFrame->UpdatePanel();
			wxString buf;
			GetDocument()->GetPrintableName(buf);
			mFrame->SetTitle(buf);
		}
		if (mFrame && mFrame->GetCanvas())
		{
			if (hint && hint->IsKindOf(CLASSINFO(CC_show_modified)))
			{
				GeneratePaths();
			}
			mFrame->GetCanvas()->Refresh();
		}
	}
}

// Clean up windows used for displaying the view.
bool
FieldView::OnClose(bool deleteWindow)
{
	SetFrame((wxFrame*)NULL);
	
	Activate(false);
	
	if (!GetDocument()->Close())
		return false;
	
	if (deleteWindow)
	{
		delete mFrame;
	}
	return true;
}


void
FieldView::OnWizardSetup(CC_show& show)
{
	wxWizard *wizard = new wxWizard(mFrame, wxID_ANY, wxT("New Show Setup Wizard"));
	// page 1:
	// set the number of points and the labels
	ShowInfoReqWizard *page1 = new ShowInfoReqWizard(wizard);
	
	// page 2:
	// choose the show mode
	ChooseShowModeWizard *page2 = new ChooseShowModeWizard(wizard);
	
	// page 3:
	// and maybe a description
	SetDescriptionWizard *page3 = new SetDescriptionWizard(wizard);
	// page 4:
	
	wxWizardPageSimple::Chain(page1, page2);
	wxWizardPageSimple::Chain(page2, page3);
	
	wizard->GetPageAreaSizer()->Add(page1);
	if (wizard->RunWizard(page1))
	{
		show.SetNumPoints(page1->GetNumberPoints(), page1->GetNumberColumns());
		show.SetPointLabel(page1->GetLabels());
		ShowMode *newmode = ShowModeList_Find(wxGetApp().GetModeList(), page2->GetValue());
		if (newmode)
		{
			show.SetMode(newmode);
		}
		show.SetDescr(page3->GetValue());
	}
	else
	{
		wxMessageBox(
					 wxT("Show setup not completed.\n")
					 wxT("You can change the number of marchers\n")
					 wxT("and show mode via the menu options"), wxT("Show not setup"), wxICON_INFORMATION|wxOK);
	}
	wizard->Destroy();
}

bool
FieldView::DoTranslatePoints(const CC_coord& delta)
{
	if (((delta.x == 0) && (delta.y == 0)) ||
		(mShow->GetSelectionList().size() == 0))
		return false;
	GetDocument()->GetCommandProcessor()->Submit(new TranslatePointsByDeltaCommand(*mShow, delta, mCurrentReferencePoint), true);
	return true;
}

bool
FieldView::DoTransformPoints(const Matrix& transmat)
{
	if (mShow->GetSelectionList().size() == 0) return false;
	GetDocument()->GetCommandProcessor()->Submit(new TransformPointsCommand(*mShow, transmat, mCurrentReferencePoint), true);
	return true;
}

bool
FieldView::DoMovePointsInLine(const CC_coord& start, const CC_coord& second)
{
	if (mShow->GetSelectionList().size() == 0) return false;
	GetDocument()->GetCommandProcessor()->Submit(new TransformPointsInALineCommand(*mShow, start, second, mCurrentReferencePoint), true);
	return true;
}

bool
FieldView::DoSetPointsSymbol(SYMBOL_TYPE sym)
{
	if (mShow->GetSelectionList().size() == 0) return false;
	GetDocument()->GetCommandProcessor()->Submit(new SetSymbolAndContCommand(*mShow, sym), true);
	return true;
}

bool
FieldView::DoSetDescription(const wxString& descr)
{
	GetDocument()->GetCommandProcessor()->Submit(new SetDescriptionCommand(*mShow, descr), true);
	return true;
}

void
FieldView::DoSetMode(const wxString& mode)
{
	GetDocument()->GetCommandProcessor()->Submit(new SetModeCommand(*mShow, mode), true);
}

void
FieldView::DoSetShowInfo(unsigned numPoints, unsigned numColumns, const std::vector<wxString>& labels)
{
	GetDocument()->GetCommandProcessor()->Submit(new SetShowInfoCommand(*mShow, numPoints, numColumns, labels), true);
}

bool
FieldView::DoSetSheetTitle(const wxString& descr)
{
	GetDocument()->GetCommandProcessor()->Submit(new SetSheetTitleCommand(*mShow, descr), true);
	return true;
}

bool
FieldView::DoSetSheetBeats(unsigned short beats)
{
	GetDocument()->GetCommandProcessor()->Submit(new SetSheetBeatsCommand(*mShow, beats), true);
	return true;
}

bool
FieldView::DoSetPointsLabel(bool right)
{
	if (mShow->GetSelectionList().size() == 0) return false;
	GetDocument()->GetCommandProcessor()->Submit(new SetLabelRightCommand(*mShow, right), true);
	return true;
}

bool
FieldView::DoSetPointsLabelFlip()
{
	if (mShow->GetSelectionList().size() == 0) return false;
	GetDocument()->GetCommandProcessor()->Submit(new SetLabelFlipCommand(*mShow), true);
	return true;
}

bool
FieldView::DoInsertSheets(const CC_show::CC_sheet_container_t& sht, unsigned where)
{
	GetDocument()->GetCommandProcessor()->Submit(new AddSheetsCommand(*mShow, sht, where), true);
	return true;
}

bool
FieldView::DoDeleteSheet(unsigned where)
{
	GetDocument()->GetCommandProcessor()->Submit(new RemoveSheetsCommand(*mShow, where), true);
	return true;
}

int
FieldView::FindPoint(CC_coord pos) const
{
	return mShow->GetCurrentSheet()->FindPoint(pos.x, pos.y, mCurrentReferencePoint);
}

CC_coord
FieldView::PointPosition(int which) const
{
	return mShow->GetCurrentSheet()->GetPosition(which, mCurrentReferencePoint);
}

void
FieldView::GoToSheet(size_t which)
{
	if (which < mShow->GetNumSheets())
	{
		mShow->SetCurrentSheet(which);
	}
}

void
FieldView::GoToNextSheet()
{
	GoToSheet(mShow->GetCurrentSheetNum() + 1);
}

void
FieldView::GoToPrevSheet()
{
	GoToSheet(mShow->GetCurrentSheetNum() - 1);
}

void
FieldView::SetReferencePoint(unsigned which)
{
	mCurrentReferencePoint = which;
	OnUpdate(this);
}

// toggle selection means toggle it as selected to unselected
// otherwise, always select it
void
FieldView::SelectWithLasso(const CC_lasso* lasso, bool toggleSelected)
{
	mShow->SelectWithLasso(*lasso, toggleSelected, mCurrentReferencePoint);
}

// Select points within rectangle
void
FieldView::SelectPointsInRect(const CC_coord& c1, const CC_coord& c2, bool toggleSelected)
{
	CC_lasso lasso(c1);
	lasso.Append(CC_coord(c1.x, c2.y));
	lasso.Append(c2);
	lasso.Append(CC_coord(c2.x, c1.y));
	lasso.End();
	SelectWithLasso(&lasso, toggleSelected);
}

void
FieldView::OnEnableDrawPaths(bool enable)
{
	mDrawPaths = enable;
	if (mDrawPaths)
	{
		GeneratePaths();
	}
	mFrame->Refresh();
}

void
FieldView::DrawPaths(wxDC& dc, const CC_sheet& sheet)
{
	if (mDrawPaths && mAnimation && mAnimation->GetNumberSheets() && (static_cast<unsigned>(mAnimation->GetNumberSheets()) > mShow->GetCurrentSheetNum()))
	{
		CC_coord origin = mShow->GetMode().Offset();
		mAnimation->GotoSheet(mShow->GetCurrentSheetNum());
		for (CC_show::SelectionList::const_iterator point = mShow->GetSelectionList().begin(); point != mShow->GetSelectionList().end(); ++point)
		{
			mAnimation->DrawPath(dc, *point, origin);
		}
	}
}

void
FieldView::GeneratePaths()
{
	mAnimation.reset(new Animation(mShow, NotifyStatus(), NotifyErrorList()));
}
