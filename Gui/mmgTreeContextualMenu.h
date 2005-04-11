/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTreeContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:29 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgTreeContextualMenu_H__
#define __mmgTreeContextualMenu_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafNode.h"

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mmgMDIChild;
class mafEvent;
class mafEventListener;
class mmgCheckTree;

//----------------------------------------------------------------------------
// mmgTreeContextualMenu :
//----------------------------------------------------------------------------
class mmgTreeContextualMenu : public wxMenu
{
public:
  mmgTreeContextualMenu();
  virtual ~mmgTreeContextualMenu();
  void SetListener(mafEventListener *Listener) {m_listener = Listener;};

	/** Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(mmgCheckTree *tree, mafView *view, mafNode *vme, bool vme_menu);

protected:
  mafView          *m_view;
  mafNode          *m_clicked_vme;
  mmgCheckTree     *m_tree;
  mafEventListener *m_listener;
  
  bool m_autosort;
  bool m_check_crypto;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual menù's selection. */
	void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif


