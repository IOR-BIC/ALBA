/*
 * wxMozillaWindow Sample Application
 * Copyright 2001 (c) Jeremiah Cornelius McCarthy. All Rights Reserved.
 */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "wxMozillaBrowser.h"
#include "wxMozillaWindow.h"

class wxMozillaDemo: public wxApp
{
public:

	virtual bool OnInit();
};

bool wxMozillaDemo::OnInit()
{
	wxMozillaWindow *MainWnd;
	MainWnd = new wxMozillaWindow();
	MainWnd->Show(TRUE);
	//wxMainWnd *MainWnd;

	//MainWnd = new wxMainWnd();
	//MainWnd->Show(TRUE);

	return TRUE;
};

IMPLEMENT_APP(wxMozillaDemo)
