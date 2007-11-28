/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewFrame.h,v $
  Language:  C++
  Date:      $Date: 2007-11-28 12:16:44 $
  Version:   $Revision: 1.6 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgViewFrame_H__
#define __mmgViewFrame_H__
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
// mmgViewFrame :
//----------------------------------------------------------------------------
class mmgViewFrame: public wxFrame , public mafObserver
{
	public:
	mmgViewFrame(wxFrame* parent, 
							wxWindowID id, 
							const wxString& title,
							const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxDefaultSize, 
							long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN 
							);
       
	~mmgViewFrame(); 
	
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
