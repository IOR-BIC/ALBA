/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTreeContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2005-11-18 14:51:26 $
  Version:   $Revision: 1.1 $
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
class mafObserver;
class mmgCheckTree;
class mafNode;
class mafVME;

//----------------------------------------------------------------------------
// mmgTreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa mmgContextualMenu
*/
class mmgTreeContextualMenu : public wxMenu
{
public:
  mmgTreeContextualMenu();
  virtual ~mmgTreeContextualMenu();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Visualize contextual men for the MDI child and selected view. */
  void ShowContextualMenu(mmgCheckTree *tree, mafView *view, mafNode *vme, bool vme_menu);

protected:
  mafView       *m_ViewActive;
  mafVME        *m_VmeActive;
  mafNode       *m_NodeActive;
  mmgCheckTree  *m_NodeTree;
  mafObserver   *m_Listener;
  
  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual men's selection. */
	void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
