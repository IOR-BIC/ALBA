/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIContextualMenu_H__
#define __mafGUIContextualMenu_H__

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafObserver;

//----------------------------------------------------------------------------
// mafGUIContextualMenu :
//----------------------------------------------------------------------------
class mafGUIContextualMenu : public wxMenu
{
public:
  mafGUIContextualMenu();
  virtual ~mafGUIContextualMenu();
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
