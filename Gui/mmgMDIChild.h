/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMDIChild.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:24 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgMDIChild_H__
#define __mmgMDIChild_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>

//#define MAX_CUSTOM_ID 20  //todo: SIL. 7-4-2005: ---- what is this ?

#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mmgMDIChild :
//----------------------------------------------------------------------------
class mmgMDIChild: public wxMDIChildFrame
{
public:
  mmgMDIChild(wxMDIParentFrame* parent, mafView *view); 
  virtual ~mmgMDIChild();
  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};
  
	/** Set the quitting flag. */
	static void OnQuit () {m_quitting=true;}; 

protected:
	/** Adjust the child size. */
  void OnSize(wxSizeEvent &event); 

	/** Send the event to select the owned view. */
  void OnSelect(wxCommandEvent &event); 

	/** Send the evenet to destroy the owned view. */
  void OnCloseWindow  (wxCloseEvent& event);

	/** Send the event to select the owned view. */
  void OnActivate (wxActivateEvent& event);

  wxWindow						*m_win;
  mafView             *m_view;
  mafEventListener    *m_Listener;
  static bool          m_quitting;

  DECLARE_EVENT_TABLE()
};
#endif
