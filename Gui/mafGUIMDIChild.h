/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMDIChild.h,v $
  Language:  C++
  Date:      $Date: 2009-06-17 13:24:52 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIMDIChild_H__
#define __mafGUIMDIChild_H__
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

/**
  Class Name: mafGUIMDIChild.
  Represents the child frame in a MDI system. In MAF it is used in combination with views that support
  viewport and eventually a gui area in which some widgets can be used, like windowing widgets.
*/
class MAF_EXPORT mafGUIMDIChild: public wxMDIChildFrame
{
public:
  /** constructor. */
  mafGUIMDIChild(wxMDIParentFrame* parent, mafView *view); 
  /** destructor. */
  virtual ~mafGUIMDIChild();
  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
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
  mafView     *m_View;
  mafObserver *m_Listener;
  static bool  m_Quitting;
  bool         m_AllowCloseFlag;
  bool         m_EnableResize;

  /** Event table declaration */
  DECLARE_EVENT_TABLE()
};
#endif
