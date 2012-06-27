/*=========================================================================

 Program: MAF2
 Module: mafGUIFrame
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIFrame_H__
#define __mafGUIFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafObserver;

/**
  Class Name: mafGUIFrame.
  A frame is a window whose position and size can be changed by the user.
  It can have borders and a title bar, and can optionally contain a menu bar, 
  toolbar and status bar. Inherit from wxFrame and override several methods of it.

*/
class MAF_EXPORT mafGUIFrame: public wxFrame
{
public:
  /** constructor. */
  mafGUIFrame (const wxString& title, const wxPoint& pos, const wxSize& size);
  /** destructor. */
  ~mafGUIFrame (); 
  
  /** Set the Listener that will receive event-notification. */
	void SetListener (mafObserver *Listener) {m_Listener = Listener;};
  
  /** Set the window shown on the Client Area, (hide the previous contents). */
	void Put (wxWindow* w);
  
  /** Redraw the frame. */
	void Update () {LayoutWindow();};

protected:
  /** Send an event with the men item's id. */
  void OnMenu(wxCommandEvent& e);
  
  /** Send an event with the men operation's id. */
	void OnMenuOp(wxCommandEvent& e);
  
  /** Send an event with the UI item's id. */
	void OnUpdateUI(wxUpdateUIEvent& e);
  
  /** Send an event with the men 'Quit' item's id. */
	void OnCloseWindow(wxCloseEvent& event);
  
  /** Redraw the frame. */
	void OnSize(wxSizeEvent& event);
  
  /** Redraw the frame's sash panel. */
	void OnSashDrag(wxSashEvent& event);
  
  /** Call LayoutWindow() method. */
	void OnLayout(wxCommandEvent& event);
  
  /** Redraw method. */
	void LayoutWindow();
  
  /** Create the status bar into the main frame. */
	void CreateStatusbar();

  mafObserver *m_Listener;
  wxWindow    *m_ClientWin;

  /** Event Table Declaration*/
DECLARE_EVENT_TABLE()
};
#endif
