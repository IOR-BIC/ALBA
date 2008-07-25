/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIViewFrame.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIViewFrame_H__
#define __mafGUIViewFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include <wx/laywin.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mafGUIViewFrame :
//----------------------------------------------------------------------------
class mafGUIViewFrame: public wxFrame , public mafObserver
{
	public:
	mafGUIViewFrame(wxFrame* parent, 
							wxWindowID id, 
							const wxString& title,
							const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxDefaultSize, 
							long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN 
							);
       
	~mafGUIViewFrame(); 
	
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};
	
  /** Set the quitting flag. */
  static void OnQuit () {m_Quitting=true;}; 

	/** Set which is the external view. */
	void SetView(mafView *view);

  /** Answer to the messages coming from bottom classes. */
	virtual void OnEvent(mafEventBase *maf_event);

protected:
	/** Send the event to destroy the owned view. */
	void OnCloseWindow  (wxCloseEvent &event);

	/** Adjust the child size. */
	void OnSize         (wxSizeEvent &event);

	/** Send the event to select the owned view. */
	void OnSelect				(wxCommandEvent &event); 

	/** Send the event to select the owned view. */
  void OnActivate     (wxActivateEvent &event);

	mafObserver *m_Listener;
	wxWindow    *m_ClientWin;
	mafView     *m_View;
  static bool  m_Quitting;

	DECLARE_EVENT_TABLE()
};
#endif
