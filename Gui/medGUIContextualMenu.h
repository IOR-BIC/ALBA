/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUIContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2009-06-17 13:05:30 $
  Version:   $Revision: 1.1.2.1 $
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

/**
  Class Name: medGUIContextualMenu.
  Represents the contexctual menu' which compares when click right button 
  over a viewport.
*/
class medGUIContextualMenu : public wxMenu
{
public:
  /** constructor. */
  medGUIContextualMenu();
  /** destructor. */
  virtual ~medGUIContextualMenu();
  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

	/** 
  Visualize contextual menu for the MDI child and selected view. */
  void ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu);		

protected:
  wxFrame     *m_ChildViewActive;
  mafView     *m_ViewActive;
  mafObserver *m_Listener;

	/** Answer contextual menu's selection. */
	void OnContextualViewMenu(wxCommandEvent& event);

  /** Event Table Declaration. */
  DECLARE_EVENT_TABLE()
};
#endif
