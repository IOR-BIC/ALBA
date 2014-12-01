/*=========================================================================

 Program: MAF2
 Module: mafGUIContextualMenu
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIContextualMenu_H__
#define __mafGUIContextualMenu_H__

#include "mafDefines.h"

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafObserver;

/**
  Class Name: mafGUIContextualMenu.
  Represents the contexctual menu' which compares when click right button 
  over a viewport.
*/
class MAF_EXPORT mafGUIContextualMenu : public wxMenu
{
public:
  /** constructor. */
  mafGUIContextualMenu();
  /** destructor. */
  virtual ~mafGUIContextualMenu();
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
