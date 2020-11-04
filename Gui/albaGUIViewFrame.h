/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIViewFrame
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIViewFrame_H__
#define __albaGUIViewFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include <wx/laywin.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaView;

//----------------------------------------------------------------------------
// albaGUIViewFrame :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaGUIViewFrame: public wxFrame , public albaObserver
{
	public:
	albaGUIViewFrame(wxFrame* parent, 
							wxWindowID id, 
							const wxString& title,
							const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxDefaultSize, 
							long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN 
							);
       
	~albaGUIViewFrame(); 
	
	void SetListener(albaObserver *Listener) {m_Listener = Listener;};
	
  /** Set the quitting flag. */
  static void OnQuit () {m_Quitting=true;}; 

	/** Set which is the external view. */
	void SetView(albaView *view);

  /** Answer to the messages coming from bottom classes. */
	virtual void OnEvent(albaEventBase *alba_event);

protected:
	/** Send the event to destroy the owned view. */
	void OnCloseWindow  (wxCloseEvent &event);

	/** Adjust the child size. */
	void OnSize         (wxSizeEvent &event);

	/** Send the event to select the owned view. */
	void OnSelect				(wxCommandEvent &event); 

	/** Send the event to select the owned view. */
  void OnActivate     (wxActivateEvent &event);

	albaObserver *m_Listener;
	wxWindow    *m_ClientWin;
	albaView     *m_View;
  static bool  m_Quitting;

	DECLARE_EVENT_TABLE()
};
#endif
