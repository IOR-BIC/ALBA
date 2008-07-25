/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUITreeContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUITreeContextualMenu_H__
#define __mafGUITreeContextualMenu_H__

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafGUIMDIChild;
class mafEvent;
class mafObserver;
class mafGUICheckTree;
class mafNode;
class mafVME;

//----------------------------------------------------------------------------
// mafGUITreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa mafGUIContextualMenu
*/
class mafGUITreeContextualMenu : public wxMenu
{
public:
  mafGUITreeContextualMenu();
  virtual ~mafGUITreeContextualMenu();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Create a contextual menu*/
  virtual void CreateContextualMenu(mafGUICheckTree *tree, mafView *view, mafNode *vme, bool vme_menu);

  /** Visualize contextual men for the MDI child and selected view. */
  void ShowContextualMenu();

protected:
  mafView       *m_ViewActive;
  mafVME        *m_VmeActive;
  mafNode       *m_NodeActive;
  mafGUICheckTree  *m_NodeTree;
  mafObserver   *m_Listener;
  
  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual men's selection. */
	virtual void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
