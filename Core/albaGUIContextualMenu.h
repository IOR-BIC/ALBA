/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIContextualMenu
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGUIContextualMenu_H__
#define __albaGUIContextualMenu_H__

#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class albaView;
class albaEvent;
class albaObserver;

/**
  class name: albaGUIContextualMenu
  Class that handle menu' opened with right button click of the mouse over the view.
*/
class ALBA_EXPORT albaGUIContextualMenu : public wxMenu, public albaServiceClient
{
public:
  /** constructor  */
  albaGUIContextualMenu();
  /** destructor  */
  virtual ~albaGUIContextualMenu();
  /** function for setting the listener of events coming from another object*/
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

	/**  Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(wxFrame *child, albaView *view, bool vme_menu);		

protected:
  wxFrame     *m_ChildViewActive;
  albaView     *m_ViewActive;
  albaObserver *m_Listener;

	/**  Answer contextual menù's selection. */
	void OnContextualViewMenu(wxCommandEvent& event);
  /** declaring event table macro */
  DECLARE_EVENT_TABLE()
};
#endif
