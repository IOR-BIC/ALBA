/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2010-02-24 09:36:41 $
  Version:   $Revision: 1.1.2.1 $
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

/**
  class name: mafGUIContextualMenu
  Class that handle menu' opened with right button click of the mouse over the view.
*/
class mafGUIContextualMenu : public wxMenu
{
public:
  /** constructor  */
  mafGUIContextualMenu();
  /** destructor  */
  virtual ~mafGUIContextualMenu();
  /** function for setting the listener of events coming from another object*/
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

	/**  Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu);		

protected:
  wxFrame     *m_ChildViewActive;
  mafView     *m_ViewActive;
  mafObserver *m_Listener;

	/**  Answer contextual menù's selection. */
	void OnContextualViewMenu(wxCommandEvent& event);
  /** declaring event table macro */
  DECLARE_EVENT_TABLE()
};
#endif
