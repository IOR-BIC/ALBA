/*=========================================================================

 Program: MAF2
 Module: mafGUITreeContextualMenu
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafSceneGraph;
class mafVME;

//----------------------------------------------------------------------------
// mafGUITreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa mafGUIContextualMenu
*/
class MAF_EXPORT mafGUITreeContextualMenu : public wxMenu
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

  mafSceneGraph *m_SceneGraph;
  
  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual men's selection. */
	virtual void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
