/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISplashScreen
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUISplashScreen_H__
#define __albaGUISplashScreen_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/splash.h>

class ALBA_EXPORT albaGUISplashScreen : public wxSplashScreen
{
public:
	enum SPLASH_SCREEN_POSITION
	{
		SPLASH_SCREEN_TOP_LEFT,
		SPLASH_SCREEN_TOP_CENTER,
		SPLASH_SCREEN_TOP_RIGHT,
		SPLASH_SCREEN_BOTTOM_LEFT,
		SPLASH_SCREEN_BOTTOM_CENTER,
		SPLASH_SCREEN_BOTTOM_RIGHT,
		SPLASH_SCREEN_CENTER
	};

	albaGUISplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds, wxWindow* parent);

	void SetText(const wxString& text,SPLASH_SCREEN_POSITION pos, wxColour color = *wxBLACK);

protected:
	wxBitmap m_noTextBitmap;
};

#endif  // __albaGUISplashScreen_H__
