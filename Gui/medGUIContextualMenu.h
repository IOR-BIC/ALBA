/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUIContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2007-10-29 14:31:58 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medGUIContextualMenu_H__
#define __medGUIContextualMenu_H__

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafObserver;

//----------------------------------------------------------------------------
// medGUIContextualMenu :
//----------------------------------------------------------------------------
class medGUIContextualMenu : public wxMenu
{
public:
  medGUIContextualMenu();
  virtual ~medGUIContextualMenu();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

	/** 
  Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu);		

protected:
  wxFrame     *m_ChildViewActive;
  mafView     *m_ViewActive;
  mafObserver *m_Listener;

	/** 
  Answer contextual menù's selection. */
	void OnContextualViewMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
