/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIButton
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUISplashScreen.h"
#include <wx/tokenzr.h>

//--------------------------------------------------------------------------------
albaGUISplashScreen::albaGUISplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds, wxWindow* parent) 
	: wxSplashScreen(bitmap, splashStyle, milliseconds, parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxSTAY_ON_TOP)
{
	m_noTextBitmap = bitmap;
}

//--------------------------------------------------------------------------------
void albaGUISplashScreen::SetText(const wxString &text, SPLASH_SCREEN_POSITION pos, wxColour color)
{
	wxBitmap bitmap(m_noTextBitmap);
	
	if (!text.empty()) {
		wxMemoryDC memDC;
		memDC.SelectObject(bitmap);
		
		wxFont font = memDC.GetFont();
		font.SetPointSize(font.GetPointSize() * 1.7);
		font.SetWeight(wxFONTWEIGHT_BOLD);
		memDC.SetFont(font);
		
		// Split text by newline character
		wxArrayString lines = wxStringTokenize(text, wxT("\n"), wxTOKEN_RET_EMPTY);
		
		// Calculate dimensions for each line and total height
		wxCoord lineHeight = 0;
		wxVector<wxCoord> lineWidths;
		
		wxCoord textWidth, textHeight, fullHeight;
		memDC.GetTextExtent(lines[0], &textWidth, &textHeight);
		lineHeight = textHeight;
		fullHeight = lineHeight * lines.GetCount();
		
		wxCoord totalHeight = lineHeight * lines.GetCount();
				
		// Create inverse color for shadow effect
		wxColour inverseColor(255 - color.Red(), 255 - color.Green(), 255 - color.Blue());
		wxColour shadowColor((color.Red() + inverseColor.Red() / 2), (color.Green() + inverseColor.Green() / 2), (color.Blue() + inverseColor.Blue() / 2));
		
		memDC.SetBackgroundMode(wxTRANSPARENT);

		int x, y;
		
		// Draw each line
		for (size_t i = 0; i < lines.GetCount(); ++i) {

			memDC.GetTextExtent(lines[i], &textWidth, &textHeight);

			switch (pos) {
			case SPLASH_SCREEN_TOP_LEFT:
				x = 5;
				y = 3+textHeight*i;
				break;
			case SPLASH_SCREEN_TOP_CENTER:
				x = (bitmap.GetWidth() - textWidth) / 2;
				y = 3+textHeight*i;
				break;
			case SPLASH_SCREEN_TOP_RIGHT:
				x = bitmap.GetWidth() - textWidth - 5;
				y = 3+textHeight*i;
				break;
			case SPLASH_SCREEN_BOTTOM_LEFT:
				x = 5;
				y = (bitmap.GetHeight() - (fullHeight + 3)) + textHeight * i;
				break;
			case SPLASH_SCREEN_BOTTOM_RIGHT:
				x = bitmap.GetWidth() - textWidth - 5;
				y = (bitmap.GetHeight() - (fullHeight +3)) + textHeight*i;
				break;
			case SPLASH_SCREEN_BOTTOM_CENTER:
				x = (bitmap.GetWidth() - textWidth) / 2;
				y = (bitmap.GetHeight() - (fullHeight + 3)) + textHeight * i;
				break;
			case SPLASH_SCREEN_CENTER:
			default:
				x = (bitmap.GetWidth() - textWidth) / 2;
				y = ((bitmap.GetHeight() - fullHeight) / 2) + textHeight * i;
				break;
			}
			
			// Draw shadow text
			memDC.SetTextForeground(shadowColor);
			memDC.DrawText(lines[i], x + 1, y + 1);
			
			// Draw main text on top
			memDC.SetTextForeground(color);
			memDC.DrawText(lines[i], x, y);
		}
		
		memDC.SelectObject(wxNullBitmap);
		
		m_window->SetBitmap(bitmap);
		m_window->Refresh();
		m_window->Update();
	}
}
