/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2005-11-10 11:46:08 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgContextualMenu_H__
#define __mmgContextualMenu_H__

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafObserver;

//----------------------------------------------------------------------------
// mmgContextualMenu :
//----------------------------------------------------------------------------
class mmgContextualMenu : public wxMenu
{
public:
  mmgContextualMenu();
  virtual ~mmgContextualMenu();
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
