/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewPage
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIViewPage_H__
#define __albaGUIViewPage_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/panel.h>

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class albaView;
class albaGUIViewArea;

/**
  Class Name: albaGUIViewPage.
  Holds the window of a view as a page of the view area (albaGUIViewArea).
  Closing the page (Close) deletes the view and removes the page from the view area.
*/
class ALBA_EXPORT albaGUIViewPage : public wxPanel
{
public:
	albaGUIViewPage(albaGUIViewArea *area, albaView *view);
	virtual ~albaGUIViewPage();

	/** Set the quitting flag: the views are no longer selected */
	static void OnQuit() { m_Quitting = true; }
	static bool IsQuitting() { return m_Quitting; }

	/**
	Allow to set the flag to avoid closing the view.
	For instance when an operation is running or something else the vertical application don't want that the view must stay open. */
	void SetAllowCloseWindow(bool allowClose) { m_AllowCloseFlag = allowClose; }

	albaView *GetView() { return m_View; }
	albaGUIViewArea *GetViewArea() { return m_ViewArea; }

protected:
	/** Adjust the size of the view window. */
	void OnSize(wxSizeEvent &event);

	/** Make the view the active one. */
	void OnSelect(wxCommandEvent &event);

	/** Delete the view and remove the page. */
	void OnCloseWindow(wxCloseEvent &event);

	wxWindow *m_Win;
	albaView *m_View;
	albaGUIViewArea *m_ViewArea;
	static bool m_Quitting;
	bool m_AllowCloseFlag;

	/** Event table declaration */
	DECLARE_EVENT_TABLE()
};
#endif
