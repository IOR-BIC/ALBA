/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" // must be first in ALBA source files
#include "albaCursor.h"
#include "wx/cursor.h"
#include "wx/app.h"

bool GLO_cursorIniialized = false;
std::vector<wxCursor *> GLO_Cursors; // Store custom cursor instances.


void albaCursor::SetCursor(wxWindow *window, const int cursor)
{
	if (!GLO_cursorIniialized)
    InitCursors();

	if (cursor > 0 && cursor < wxCURSOR_MAX + GLO_Cursors.size())
		window->SetCursor(*GLO_Cursors[cursor-1]);
}

//----------------------------------------------------------------------------
void albaCursor::InitCursors()
{
	DeleteCursors();

	//Init stock cursors
	for(int i = wxCURSOR_NONE+1; i< wxCURSOR_MAX; i++)
		GLO_Cursors.push_back(new wxCursor((wxStockCursor)i));

	static char *rotation_xpm[] = {
	"24 24 33 1",
	" 	c None",
	".	c #000000",
	"+	c #C3C3C3",
	"@	c #FFFFFF",
	"#	c #B1B1B1",
	"$	c #F5F5F5",
	"%	c #7E7E7E",
	"&	c #F9F9F9",
	"*	c #DDDDDD",
	"=	c #E6E6E6",
	"-	c #E3E3E3",
	";	c #CACACA",
	">	c #DFDDDD",
	",	c #F3F3F3",
	"'	c #888888",
	")	c #CFCFCF",
	"!	c #666666",
	"~	c #B4B4B4",
	"{	c #E8E8E8",
	"]	c #DBDBDB",
	"^	c #EAEAEA",
	"/	c #ACACAC",
	"(	c #F7F9F9",
	"_	c #817E81",
	":	c #B6B6B6",
	"<	c #F7F7F7",
	"[	c #C5C5C5",
	"}	c #F7F3F3",
	"|	c #D4D4D4",
	"1	c #A9A9A9",
	"2	c #B9B9B9",
	"3	c #575757",
	"4	c #575752",
	"                        ",
	"                  ..    ",
	"                 .+.    ",
	"                ..@#.   ",
	"                .$@@..  ",
	"               .%@@@&.  ",
	"              ..*=@$-%. ",
	"               . .;.... ",
	"                 .>.    ",
	"                 .,.    ",
	"                 .=.    ",
	"                .').    ",
	"                .=.     ",
	"               .!~.     ",
	"       .      ..{.      ",
	"     ...     ..$..      ",
	"    ..].   ..!^..       ",
	"  ../@(...._=:..        ",
	" ..<@@@^[}{|..          ",
	" ..1@@@23....           ",
	"   .4@&.                ",
	"    ..).                ",
	"      ..                ",
	"                        " };

	wxImage rotation_img(rotation_xpm);
	rotation_img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 20);
	rotation_img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 4);


  // Initialize custom cursors if needed. For example, if CURSOR_ROTATE is defined as a custom cursor:
  GLO_Cursors.push_back(new wxCursor(rotation_img)); // Placeholder for CURSOR_ROTATE
  // Add more custom cursors as needed.
	GLO_cursorIniialized = true;
}

//----------------------------------------------------------------------------
void albaCursor::DeleteCursors()
{
	for (int i = 0; i < GLO_Cursors.size(); i++)
	{
		delete GLO_Cursors[i];
	}
	GLO_Cursors.clear();
}
