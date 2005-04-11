/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFrame.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:22 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgFrame_H__
#define __mmgFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafEventListener;

//----------------------------------------------------------------------------
// mmgFrame :
//----------------------------------------------------------------------------
class mmgFrame: public wxFrame
{
public:
         mmgFrame (const wxString& title, const wxPoint& pos, const wxSize& size);
        ~mmgFrame (); 
  
	void SetListener (mafEventListener *Listener) {m_Listener = Listener;};
  
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

  mafEventListener *m_Listener;
  wxWindow         *m_clientwin;

DECLARE_EVENT_TABLE()
};
#endif
