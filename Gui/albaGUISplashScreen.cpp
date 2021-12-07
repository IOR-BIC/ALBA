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

//--------------------------------------------------------------------------------
albaGUISplashScreen::albaGUISplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds, wxWindow* parent) 
	: wxSplashScreen(bitmap, splashStyle, milliseconds, parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxSTAY_ON_TOP)
{
	m_noTextBitmap = bitmap;
}

//--------------------------------------------------------------------------------
void albaGUISplashScreen::SetText(const wxString& text, int x, int y, wxColour color)
{
	wxBitmap bitmap(m_noTextBitmap);
	
	if (!text.empty()) {
		wxMemoryDC memDC;

		memDC.SelectObject(bitmap);

		memDC.SetBackgroundMode(wxTRANSPARENT);
		memDC.SetTextForeground(color);
		memDC.DrawText(text, x, y);

		memDC.SelectObject(wxNullBitmap);
	}

	m_window->SetBitmap(bitmap);
	m_window->Refresh();
	m_window->Update();
}
