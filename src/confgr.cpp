/*
 * confgr.cpp
 * Basic configuration initialization for all systems
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

#include <wx/utils.h>
#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/palette.h>
#include <wx/pen.h>
#include <wx/config.h>
#include <wx/confbase.h>
#include <ctype.h>
#include <string>

#include "calchartapp.h"
#include "confgr.h"
#include "modes.h"
#include "show.h"

const wxString ColorNames[COLOR_NUM] =
{
	wxT("FIELD"),
	wxT("FIELD DETAIL"),
	wxT("FIELD TEXT"),
	wxT("POINT"),
	wxT("POINT TEXT"),
	wxT("HILIT POINT"),
	wxT("HILIT POINT TEXT"),
	wxT("REF POINT"),
	wxT("REF POINT TEXT"),
	wxT("HILIT REF POINT"),
	wxT("HILIT REF POINT TEXT"),
	wxT("ANIM FRONT"),
	wxT("ANIM BACK"),
	wxT("ANIM SIDE"),
	wxT("HILIT ANIM FRONT"),
	wxT("HILIT ANIM BACK"),
	wxT("HILIT ANIM SIDE"),
	wxT("ANIM COLLISION"),
	wxT("SHAPES")
};

const wxString DefaultColors[COLOR_NUM] =
{
	wxT("FOREST GREEN"),
	wxT("WHITE"),
	wxT("BLACK"),
	wxT("WHITE"),
	wxT("BLACK"),
	wxT("YELLOW"),
	wxT("BLACK"),
	wxT("PURPLE"),
	wxT("BLACK"),
	wxT("PURPLE"),
	wxT("BLACK"),
	wxT("WHITE"),
	wxT("YELLOW"),
	wxT("SKY BLUE"),
	wxT("RED"),
	wxT("RED"),
	wxT("RED"),
	wxT("PURPLE"),
	wxT("RED")
};

const int DefaultPenWidth[COLOR_NUM] =
{
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
};

// constants for behavior:
// Autosave
static const wxString kAutosaveIntervalKey = wxT("AutosaveInterval");
static const int kAutosaveIntervalValue = 60;
static const wxString kAutosaveDirKey = wxT("AutosaveDir");
static const wxString kAutosaveDirValue = AUTOSAVE_VAR;

static const wxString kMainFrameZoomKey = wxT("MainFrameZoom");
static const long kMainFrameZoomValue = 5;
static const wxString kMainFrameWidthKey = wxT("MainFrameWidth");
static const unsigned int kMainFrameWidthValue = 600;
static const wxString kMainFrameHeightKey = wxT("MainFrameHeight");
static const unsigned int kMainFrameHeightValue = 450;
static const wxString kPrintFileKey = wxT("PrintFile");
static const wxString kPrintFileValue = wxT("LPT1");
static const wxString kPrintCmdKey = wxT("PrintCmd");
static const wxString kPrintCmdValue = wxT("lpr");
static const wxString kPrintOptsKey = wxT("PrintOpts");
static const wxString kPrintOptsValue = wxT("");
static const wxString kPrintViewCmdKey = wxT("PrintViewCmd");
static const wxString kPrintViewCmdValue = wxT("ghostview");
static const wxString kPrintViewOptsKey = wxT("PrintViewOpts");
static const wxString kPrintViewOptsValue = wxT("");

static const wxString kPageWidthKey = wxT("PageWidth");
static const float kPageWidthValue = 7.5;
static const wxString kPageHeightKey = wxT("PageHeight");
static const float kPageHeightValue = 10.0;
static const wxString kPageOffsetXKey = wxT("PageOffsetX");
static const float kPageOffsetXValue = 0.5;
static const wxString kPageOffsetYKey = wxT("PageOffsetY");
static const float kPageOffsetYValue = 0.5;

// printing controls
static const wxString kHeadFontKey = wxT("HeadFont");
static const wxString kHeadFontValue = wxT("Helvetica-Bold");
static const wxString kMainFontKey = wxT("MainFont");
static const wxString kMainFontValue = wxT("Helvetica");
static const wxString kNumberFontKey = wxT("NumberFont");
static const wxString kNumberFontValue = wxT("Helvetica");
static const wxString kContFontKey = wxT("ContFont");
static const wxString kContFontValue = wxT("Courier");
static const wxString kBoldFontKey = wxT("BoldFont");
static const wxString kBoldFontValue = wxT("Courier-Bold");
static const wxString kItalFontKey = wxT("ItalFont");
static const wxString kItalFontValue = wxT("Courier-Italic");
static const wxString kBoldItalFontKey = wxT("BoldItalFont");
static const wxString kBoldItalFontValue = wxT("Courier-BoldItalic");
static const wxString kPaperLengthKey = wxT("PaperLength");
static const float kPaperLengthValue = 11.0;
static const wxString kHeaderSizeKey = wxT("HeaderSize");
static const float kHeaderSizeValue = 3.0;
static const wxString kYardsSizeKey = wxT("YardsSize");
static const float kYardsSizeValue = 1.5;
static const wxString kTextSizeKey = wxT("TextSize");
static const float kTextSizeValue = 10.0;
static const wxString kDotRatioKey = wxT("DotRatio");
static const float kDotRatioValue = 1.0;
static const wxString kNumRatioKey = wxT("NumRatio");
static const float kNumRatioValue = 1.2;
static const wxString kPLineRatioKey = wxT("PLineRatio");
static const float kPLineRatioValue = 1.0;
static const wxString kSLineRatioKey = wxT("SLineRatio");
static const float kSLineRatioValue = 1.0;
static const wxString kContRatioKey = wxT("ContRatio");
static const float kContRatioValue = 0.25;

template <typename T>
T GetConfigValue(const wxString& key, const T& def)
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/CalChart"));
	return config->Read(key, def);
}

// default value need to check if we need to set a value
template <typename T>
void SetConfigValue(const wxString& key, const T& value, const T& def)
{
	// don't write if we don't have to
	if (GetConfigValue<T>(key, def) == value)
		return;
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/CalChart"));
	config->Write(key, value);
	config->Flush();
}

template <typename T>
void ClearConfigValue(const wxString& key)
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/CalChart"));
	config->DeleteEntry(key);
	config->Flush();
}

#define IMPLEMENT_CONFIGURATION_FUNCTIONS( KeyName, Type ) \
Type GetConfiguration_ ## KeyName () { return GetConfigValue<Type>( k ## KeyName ## Key, k ## KeyName ## Value); } \
void SetConfiguration_ ## KeyName (const Type& v) { return SetConfigValue<Type>( k ## KeyName ## Key, v, k ## KeyName ## Value); } \
void ClearConfiguration_ ## KeyName () { return ClearConfigValue<Type>( k ## KeyName ## Key ); }

// printing controls
wxString yard_text[MAX_YARD_LINES] =
{
	wxT("N"), wxT("M"), wxT("L"), wxT("K"), wxT("J"), wxT("I"), wxT("H"), wxT("G"), wxT("F"), wxT("E"), wxT("D"), wxT("C"), wxT("B"), wxT("A"),
	wxT("-10"), wxT("-5"), wxT("0"), wxT("5"), wxT("10"), wxT("15"), wxT("20"), wxT("25"), wxT("30"), wxT("35"), wxT("40"), wxT("45"), wxT("50"),
	wxT("45"), wxT("40"), wxT("35"), wxT("30"), wxT("25"), wxT("20"), wxT("15"), wxT("10"), wxT("5"), wxT("0"), wxT("-5"), wxT("-10"),
	wxT("A"), wxT("B"), wxT("C"), wxT("D"), wxT("E"), wxT("F"), wxT("G"), wxT("H"), wxT("I"), wxT("J"), wxT("K"), wxT("L"), wxT("M"), wxT("N")
};

const wxString yard_text_index[MAX_YARD_LINES] =
{
	wxT("N"), wxT("M"), wxT("L"), wxT("K"), wxT("J"), wxT("I"), wxT("H"), wxT("G"), wxT("F"), wxT("E"), wxT("D"), wxT("C"), wxT("B"), wxT("A"),
	wxT("-10"), wxT("-5"), wxT("0"), wxT("5"), wxT("10"), wxT("15"), wxT("20"), wxT("25"), wxT("30"), wxT("35"), wxT("40"), wxT("45"), wxT("50"),
	wxT("45"), wxT("40"), wxT("35"), wxT("30"), wxT("25"), wxT("20"), wxT("15"), wxT("10"), wxT("5"), wxT("0"), wxT("-5"), wxT("-10"),
	wxT("A"), wxT("B"), wxT("C"), wxT("D"), wxT("E"), wxT("F"), wxT("G"), wxT("H"), wxT("I"), wxT("J"), wxT("K"), wxT("L"), wxT("M"), wxT("N")
};

wxString spr_line_text[MAX_SPR_LINES] =
{
	wxT("A"), wxT("B"), wxT("C"), wxT("D"), wxT("E"),
};

const wxString spr_line_text_index[MAX_SPR_LINES] =
{
	wxT("A"), wxT("B"), wxT("C"), wxT("D"), wxT("E"),
};

#define IMPLEMENT_CONFIGURATION_FUNCTIONS( KeyName, Type ) \
Type GetConfiguration_ ## KeyName () { return GetConfigValue<Type>( k ## KeyName ## Key, k ## KeyName ## Value); } \
void SetConfiguration_ ## KeyName (const Type& v) { return SetConfigValue<Type>( k ## KeyName ## Key, v, k ## KeyName ## Value); } \
void ClearConfiguration_ ## KeyName () { return ClearConfigValue<Type>( k ## KeyName ## Key ); }


IMPLEMENT_CONFIGURATION_FUNCTIONS( AutosaveDir, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( AutosaveInterval, long);

IMPLEMENT_CONFIGURATION_FUNCTIONS( MainFrameZoom, long);
IMPLEMENT_CONFIGURATION_FUNCTIONS( MainFrameWidth, long);
IMPLEMENT_CONFIGURATION_FUNCTIONS( MainFrameHeight, long);

// printing
IMPLEMENT_CONFIGURATION_FUNCTIONS( PrintFile, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PrintCmd, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PrintOpts, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PrintViewCmd, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PrintViewOpts, wxString);

IMPLEMENT_CONFIGURATION_FUNCTIONS( PageWidth, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PageHeight, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PageOffsetX, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PageOffsetY, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PaperLength, float);

IMPLEMENT_CONFIGURATION_FUNCTIONS( HeadFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( MainFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( NumberFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( ContFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( BoldFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( ItalFont, wxString);
IMPLEMENT_CONFIGURATION_FUNCTIONS( BoldItalFont, wxString);

IMPLEMENT_CONFIGURATION_FUNCTIONS( HeaderSize, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( YardsSize, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( TextSize, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( DotRatio, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( NumRatio, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( PLineRatio, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( SLineRatio, float);
IMPLEMENT_CONFIGURATION_FUNCTIONS( ContRatio, float);

// Color is more complicated, we use functions for setting that
wxPalette *CalChartPalette;
const wxPen *CalChartPens[COLOR_NUM];
const wxBrush *CalChartBrushes[COLOR_NUM];

///// Show mode configuration /////

const wxString kShowModeStrings[SHOWMODE_NUM] =
{
	wxT("Standard"),
	wxT("Full Field"),
	wxT("Tunnel"),
	wxT("Old Field"),
	wxT("Pro Field")
};

// What values mean:
// whash ehash (steps from west sideline)
// left top right bottom (border in steps)
// x y w h (region of the field to use, in steps)
const long kShowModeDefaultValues[SHOWMODE_NUM][kShowModeValues] =
{
	{ 32, 52, 8, 8, 8, 8, -80, -42, 160, 84 },
	{ 32, 52, 8, 8, 8, 8, -96, -42, 192, 84 },
	{ 32, 52, 8, 8, 8, 8, 16, -42, 192, 84 },
	{ 28, 52, 8, 8, 8, 8, -80, -42, 160, 84 },
	{ 36, 48, 8, 8, 8, 8, -80, -42, 160, 84 }
};

const wxString kSpringShowModeStrings[SPRINGSHOWMODE_NUM] =
{
	wxT("Zellerbach"),
};

// what values mean
// X (hex digit of which yard lines to print:
//   8 = left, 4 = right, 2 = above, 1 = below)
// left top right bottom (border in steps)
// x y w h (region of the field to use, in steps)
// x y w h (size of stage EPS file as per BoundingBox)
// x y w h (where to put the field on the stage, depends on the EPS file)
// l r t b (location of yard line text's inside edge, depends on the EPS file)
const long kSpringShowModeDefaultValues[SPRINGSHOWMODE_NUM][kSpringShowModeValues] =
{
	{ 0xD, 8, 8, 8, 8, -16, -30, 32, 28, 0, 0, 571, 400, 163, 38, 265, 232, 153, 438, 270, 12 }
};

void GetConfigurationShowMode(size_t which, long values[kShowModeValues])
{
	for (size_t i = 0; i < kShowModeValues; ++i)
	{
		values[i] = kShowModeDefaultValues[which][i];
	}

	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SHOWMODES"));
	if (!config->Exists(kShowModeStrings[which]))
	{
		config->SetPath(kShowModeStrings[which]);
		values[0] = config->Read(wxT("whash"), values[0]);
		values[1] = config->Read(wxT("ehash"), values[1]);
		values[2] = config->Read(wxT("bord1_x"), values[2]);
		values[3] = config->Read(wxT("bord1_y"), values[3]);
		values[4] = config->Read(wxT("bord2_x"), values[4]);
		values[5] = config->Read(wxT("bord2_y"), values[5]);
		values[6] = config->Read(wxT("size_x"), values[6]);
		values[7] = config->Read(wxT("size_y"), values[7]);
		values[8] = config->Read(wxT("offset_x"), values[8]);
		values[9] = config->Read(wxT("offset_y"), values[9]);
	}
}

void ReadConfigurationShowMode()
{
	for (size_t i=0; i<SHOWMODE_NUM; ++i)
	{
		long values[kShowModeValues];
		GetConfigurationShowMode(i, values);
		unsigned short whash = values[0];
		unsigned short ehash = values[1];
		CC_coord bord1, bord2;
		bord1.x = INT2COORD(values[2]);
		bord1.y = INT2COORD(values[3]);
		bord2.x = INT2COORD(values[4]);
		bord2.y = INT2COORD(values[5]);
		CC_coord size, offset;
		offset.x = INT2COORD(-values[6]);
		offset.y = INT2COORD(-values[7]);
		size.x = INT2COORD(values[8]);
		size.y = INT2COORD(values[9]);

		wxGetApp().GetModeList().Add(new ShowModeStandard(kShowModeStrings[i], bord1, bord2, size, offset, whash, ehash));
	}
}

void GetConfigurationSpringShowMode(size_t which, long values[kShowModeValues])
{
	for (size_t i = 0; i < kShowModeValues; ++i)
	{
		values[i] = kSpringShowModeDefaultValues[which][i];
	}

	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SPRINGSHOWMODES"));
	if (!config->Exists(kSpringShowModeStrings[which]))
	{
		config->SetPath(kSpringShowModeStrings[which]);
		values[0] = config->Read(wxT("which_spr_yards"), values[0]);
		values[1] = config->Read(wxT("bord1_x"), values[1]);
		values[2] = config->Read(wxT("bord1_y"), values[2]);
		values[3] = config->Read(wxT("bord2_x"), values[3]);
		values[4] = config->Read(wxT("bord2_y"), values[4]);

		values[5] = config->Read(wxT("mode_steps_x"), values[5]);
		values[6] = config->Read(wxT("mode_steps_y"), values[6]);
		values[7] = config->Read(wxT("mode_steps_w"), values[7]);
		values[8] = config->Read(wxT("mode_steps_h"), values[8]);
		values[9] = config->Read(wxT("eps_stage_x"), values[9]);
		values[10] = config->Read(wxT("eps_stage_y"), values[10]);
		values[11] = config->Read(wxT("eps_stage_w"), values[11]);
		values[12] = config->Read(wxT("eps_stage_h"), values[12]);
		values[13] = config->Read(wxT("eps_field_x"), values[13]);
		values[14] = config->Read(wxT("eps_field_y"), values[14]);
		values[15] = config->Read(wxT("eps_field_w"), values[15]);
		values[16] = config->Read(wxT("eps_field_h"), values[16]);
		values[17] = config->Read(wxT("eps_text_left"), values[17]);
		values[18] = config->Read(wxT("eps_text_right"), values[18]);
		values[19] = config->Read(wxT("eps_text_top"), values[19]);
		values[21] = config->Read(wxT("eps_text_bottom"), values[20]);
	}
}

void ReadConfigurationSpringShowMode()
{
	for (size_t i=0; i<SPRINGSHOWMODE_NUM; ++i)
	{
		long values[kSpringShowModeValues];
		GetConfigurationSpringShowMode(i, values);
		unsigned char which_spr_yards = values[0];
		CC_coord bord1, bord2;
		bord1.x = INT2COORD(values[1]);
		bord1.y = INT2COORD(values[2]);
		bord2.x = INT2COORD(values[3]);
		bord2.y = INT2COORD(values[4]);

		short mode_steps_x = values[5];
		short mode_steps_y = values[6];
		short mode_steps_w = values[7];
		short mode_steps_h = values[8];
		short eps_stage_x = values[9];
		short eps_stage_y = values[10];
		short eps_stage_w = values[11];
		short eps_stage_h = values[12];
		short eps_field_x = values[13];
		short eps_field_y = values[14];
		short eps_field_w = values[15];
		short eps_field_h = values[16];
		short eps_text_left = values[17];
		short eps_text_right = values[18];
		short eps_text_top = values[19];
		short eps_text_bottom = values[20];
		wxGetApp().GetModeList().Add(new ShowModeSprShow(kSpringShowModeStrings[i], bord1, bord2,
					which_spr_yards,
					mode_steps_x, mode_steps_y,
					mode_steps_w, mode_steps_h,
					eps_stage_x, eps_stage_y,
					eps_stage_w, eps_stage_h,
					eps_field_x, eps_field_y,
					eps_field_w, eps_field_h,
					eps_text_left, eps_text_right,
					eps_text_top, eps_text_bottom));

	}
}

void SetConfigurationShowMode(size_t which, const long values[kShowModeValues])
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SHOWMODES"));
	config->SetPath(kShowModeStrings[which]);
	config->Write(wxT("whash"), values[0]);
	config->Write(wxT("ehash"), values[1]);
	config->Write(wxT("bord1_x"), COORD2INT(values[2]));
	config->Write(wxT("bord1_y"), COORD2INT(values[3]));
	config->Write(wxT("bord2_x"), COORD2INT(values[4]));
	config->Write(wxT("bord2_y"), COORD2INT(values[5]));
	config->Write(wxT("size_x"), -COORD2INT(values[6]));
	config->Write(wxT("size_y"), -COORD2INT(values[7]));
	config->Write(wxT("offset_x"), COORD2INT(values[8]));
	config->Write(wxT("offset_y"), COORD2INT(values[9]));
	config->Flush();
}

void SetConfigurationSpringShowMode(size_t which, const long values[kSpringShowModeValues])
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SPRINGSHOWMODES"));
	config->SetPath(kSpringShowModeStrings[which]);
	config->Write(wxT("which_spr_yards"), values[0]);
	config->Write(wxT("bord1_x"), COORD2INT(values[1]));
	config->Write(wxT("bord1_y"), COORD2INT(values[2]));
	config->Write(wxT("bord2_x"), COORD2INT(values[3]));
	config->Write(wxT("bord2_y"), COORD2INT(values[4]));

	config->Write(wxT("mode_steps_x"), values[5]);
	config->Write(wxT("mode_steps_y"), values[6]);
	config->Write(wxT("mode_steps_w"), values[7]);
	config->Write(wxT("mode_steps_h"), values[8]);
	config->Write(wxT("eps_stage_x"), values[9]);
	config->Write(wxT("eps_stage_y"), values[10]);
	config->Write(wxT("eps_stage_w"), values[11]);
	config->Write(wxT("eps_stage_h"), values[12]);
	config->Write(wxT("eps_field_x"), values[13]);
	config->Write(wxT("eps_field_y"), values[14]);
	config->Write(wxT("eps_field_w"), values[15]);
	config->Write(wxT("eps_field_h"), values[16]);
	config->Write(wxT("eps_text_left"), values[17]);
	config->Write(wxT("eps_text_right"), values[18]);
	config->Write(wxT("eps_text_top"), values[19]);
	config->Write(wxT("eps_text_bottom"), values[20]);
	config->Flush();
}

void ClearConfigurationShowMode(size_t which)
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SHOWMODES"));
	config->DeleteEntry(kShowModeStrings[which]);
}

void ClearConfigurationSpringShowMode(size_t which)
{
	wxConfigBase *config = wxConfigBase::Get();
	config->SetPath(wxT("/SPRINGSHOWMODES"));
	config->DeleteEntry(kSpringShowModeStrings[which]);
}

///// Color Configuration /////
void ReadConfigColor()
{
	wxConfigBase *config = wxConfigBase::Get();
	
	// read out the color configuration:
	config->SetPath(wxT("/COLORS"));
	for (size_t i=0; i<COLOR_NUM; i++)
	{
		wxColour c;
		wxString rbuf = ColorNames[i] + wxT("_Red");
		wxString gbuf = ColorNames[i] + wxT("_Green");
		wxString bbuf = ColorNames[i] + wxT("_Blue");
		if (config->Exists(rbuf) && config->Exists(gbuf) && config->Exists(bbuf))
		{
			long r = config->Read(rbuf, 0l);
			long g = config->Read(gbuf, 0l);
			long b = config->Read(bbuf, 0l);
			c = wxColour(r, g, b);
		}
		else
		{
			c = wxColour(DefaultColors[i]);
		}
		CalChartBrushes[i] = wxTheBrushList->FindOrCreateBrush(c, wxSOLID);
		// store widths in a subgroup
		config->SetPath(wxT("WIDTH"));
		long width = DefaultPenWidth[i];
		config->Read(ColorNames[i], &width);
		if (CalChartPens[i] == NULL)
		{
			CalChartPens[i] = wxThePenList->FindOrCreatePen(c, width, wxSOLID);
		}
		config->SetPath(wxT(".."));
	}
}

void SetConfigColor(size_t selection)
{
	wxConfigBase *config = wxConfigBase::Get();
	
	// read out the color configuration:
	config->SetPath(wxT("/COLORS"));
	wxString rbuf = ColorNames[selection] + wxT("_Red");
	config->Write(rbuf, static_cast<long>(CalChartBrushes[selection]->GetColour().Red()));
	wxString gbuf = ColorNames[selection] + wxT("_Green");
	config->Write(gbuf, static_cast<long>(CalChartBrushes[selection]->GetColour().Green()));
	wxString bbuf = ColorNames[selection] + wxT("_Blue");
	config->Write(bbuf, static_cast<long>(CalChartBrushes[selection]->GetColour().Blue()));
	config->SetPath(wxT("WIDTH"));
	config->Write(ColorNames[selection], CalChartPens[selection]->GetWidth());
	config->Flush();
}

void ClearConfigColor(size_t selection)
{
	wxConfigBase *config = wxConfigBase::Get();
	
	config->SetPath(wxT("/COLORS"));
	wxString rbuf = ColorNames[selection] + wxT("_Red");
	config->DeleteEntry(rbuf);
	wxString gbuf = ColorNames[selection] + wxT("_Green");
	config->DeleteEntry(gbuf);
	wxString bbuf = ColorNames[selection] + wxT("_Blue");
	config->DeleteEntry(bbuf);
	config->SetPath(wxT("WIDTH"));
	config->DeleteEntry(ColorNames[selection]);
	config->Flush();
}

void ReadConfigYardlines()
{
	for (size_t i = 0; i < MAX_YARD_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("YardLines_%d"), i);
		yard_text[i] = GetConfigValue<wxString>(key, yard_text_index[i]);
	}
}

void SetConfigShowYardline()
{
	for (size_t i = 0; i < MAX_YARD_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("YardLines_%d"), i);
		SetConfigValue<wxString>(key, yard_text[i], yard_text_index[i]);
	}
}

void ClearConfigShowYardline()
{
	for (size_t i = 0; i < MAX_YARD_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("YardLines_%d"), i);
		ClearConfigValue<wxString>(key);
	}
	ReadConfigYardlines();
}

void ReadConfigSpringYardlines()
{
	for (size_t i = 0; i < MAX_SPR_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("SpringShowYardLines_%d"), i);
		spr_line_text[i] = GetConfigValue<wxString>(key, spr_line_text_index[i]);
	}
}

void SetConfigSpringShowYardline()
{
	for (size_t i = 0; i < MAX_SPR_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("SpringShowYardLines_%d"), i);
		SetConfigValue<wxString>(key, spr_line_text[i], spr_line_text_index[i]);
	}
}

void ClearConfigSpringShowYardline()
{
	for (size_t i = 0; i < MAX_SPR_LINES; ++i)
	{
		wxString key;
		key.Printf(wxT("SpringShowYardLines_%d"), i);
		ClearConfigValue<wxString>(key);
	}
	ReadConfigSpringYardlines();
}

void InitializeColorePalette()
{
	// create a color palette
	int i, j;
	int n = 0;
	unsigned char rd[COLOR_NUM], gd[COLOR_NUM], bd[COLOR_NUM];
	CalChartPalette = new wxPalette();
	for (i = 0; i < COLOR_NUM; i++)
	{
		for (j = 0; j < i; j++)
		{
			const wxColour *c1, *c2;
			c1 = &CalChartPens[i]->GetColour();
			if (j >= 0)
			{
				c2 = &CalChartPens[j]->GetColour();
			}
			else
			{
				if (j < -1)
				{
					c2 = wxBLACK;
				}
				else
				{
					c2 = wxWHITE;
				}
			}
			if ((c1->Red() == c2->Red()) &&
				(c1->Green() == c2->Green()) &&
				(c1->Blue() == c2->Blue()))
			{
				break;
			}
		}
		if (i == j)
		{
			rd[n] = CalChartPens[i]->GetColour().Red();
			gd[n] = CalChartPens[i]->GetColour().Green();
			bd[n] = CalChartPens[i]->GetColour().Blue();
			n++;
		}
	}
	CalChartPalette->Create(n, rd, gd, bd);
}

void ReadConfig()
{
	ReadConfigColor();
	ReadConfigurationShowMode();
	ReadConfigurationSpringShowMode();
	ReadConfigYardlines();
	ReadConfigSpringYardlines();
	InitializeColorePalette();
}


int ReadDOSline(FILE *fp, wxString& str)
{
	char buf[1024];
	if (fgets(buf, sizeof(buf), fp) == NULL)
		return 0;
	int c = strlen(buf);
// chomp like, keep removing \r and \n from the end of a line
	while(c && ((buf[c-1] == '\r') || (buf[c-1] == '\n')))
	{
		buf[c-1] = '\0';
		c--;
	}
	str = wxString::FromUTF8(buf);
	return c;
}

