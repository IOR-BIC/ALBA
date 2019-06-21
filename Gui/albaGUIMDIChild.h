/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMDIChild
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUIMDIChild_H__
#define __albaGUIMDIChild_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>

//#define MAX_CUSTOM_ID 20  //todo: SIL. 7-4-2005: ---- what is this ?

#include "albaEvent.h"
//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class albaView;

/**
  Class Name: albaGUIMDIChild.
  Represents the child frame in a MDI system. In ALBA it is used in combination with views that support
  viewport and eventually a gui area in which some widgets can be used, like windowing widgets.
*/
class ALBA_EXPORT albaGUIMDIChild: public wxMDIChildFrame
{
public:
  /** constructor. */
  albaGUIMDIChild(wxMDIParentFrame* parent, albaView *view); 
  /** destructor. */
  virtual ~albaGUIMDIChild();
  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};
  
	/** Set the quitting flag. */
	static void OnQuit () {m_Quitting=true;}; 

  /** 
  Allow to set the flag to avoid closing the view. 
  For instance when an operation is running or something else the vertical application don't want that the view must stay open. */
  void SetAllowCloseWindow(bool allow_close) {m_AllowCloseFlag = allow_close;};

  /** Enable/disable resizing of the view.*/
  void EnableResize(bool enable = true);

protected:
	/** Adjust the child size. */
  void OnSize(wxSizeEvent &event); 

	/** Send the event to select the owned view. */
  void OnSelect(wxCommandEvent &event); 

	/** Send the event to destroy the owned view. */
  void OnCloseWindow  (wxCloseEvent& event);

	/** Send the event to select the owned view. */
  void OnActivate (wxActivateEvent& event);

  /** Used to maximize view in remote application.*/
  void OnMaximize(wxMaximizeEvent &event);

  wxWindow		*m_Win;
  albaView     *m_View;
  albaObserver *m_Listener;
  static bool  m_Quitting;
  bool         m_AllowCloseFlag;
  bool         m_EnableResize;

  /** Event table declaration */
  DECLARE_EVENT_TABLE()
};
#endif
