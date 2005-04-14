/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTreeContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 15:15:49 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mmgTreeContextualMenu_H__
#define __mmgTreeContextualMenu_H__

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mmgMDIChild;
class mafEvent;
class mafEventListener;
class mmgCheckTree;
class mafNode;
class mafVME;

//----------------------------------------------------------------------------
// mmgTreeContextualMenu :
//----------------------------------------------------------------------------
class mmgTreeContextualMenu : public wxMenu
{
public:
  mmgTreeContextualMenu();
  virtual ~mmgTreeContextualMenu();
  void SetListener(mafEventListener *Listener) {m_Listener = Listener;};

	/** Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(mmgCheckTree *tree, mafView *view, mafNode *vme, bool vme_menu);

protected:
  mafView          *m_view;
  mafVME           *m_clicked_vme;
  mafNode          *m_clicked_node;
  mmgCheckTree     *m_tree;
  mafEventListener *m_Listener;
  
  bool m_autosort;
  bool m_check_crypto;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual menù's selection. */
	void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif