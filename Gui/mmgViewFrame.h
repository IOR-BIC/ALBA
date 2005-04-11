/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgViewFrame.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:30 $
  Version:   $Revision: 1.2 $
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
#include <wx/laywin.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mmgViewFrame :
//----------------------------------------------------------------------------
class mmgViewFrame: public wxFrame , public mafEventListener
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
	
	void SetListener(mafEventListener *Listener) {m_Listener = Listener;};
	
	/** Set which is the external view. */
	void SetView(mafView *view);

  /** Answer to the messages coming from bottom classes. */
	virtual void OnEvent(mafEvent& e);

protected:
	/** Send the evenet to destroy the owned view. */
	void OnCloseWindow  (wxCloseEvent&	event);

	/** Adjust the child size. */
	void OnSize         (wxSizeEvent&		event);

	/** Send the event to select the owned view. */
	void OnSelect				(wxCommandEvent&event); 

	/** Send the event to select the owned view. */
  void OnActivate     (wxActivateEvent& event);

	mafEventListener *m_Listener;
	wxWindow         *m_clientwin;
	mafView          *m_view;
  static bool       m_quitting;

	DECLARE_EVENT_TABLE()
};
#endif
