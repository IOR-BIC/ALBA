/*=========================================================================

 Program: MAF2
 Module: mafGUIContextualMenu
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafGUIContextualMenu : public wxMenu
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
