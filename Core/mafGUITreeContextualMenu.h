#include "mafOpManager.h"
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

#include "mafServiceClient.h"
#include "mafAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafGUIMDIChild;
class mafEvent;
class mafObserver;
class mafGUICheckTree;
class mafVME;
class mafSceneGraph;
class mafVME;
class mafOpManager;

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
enum TREE_CONTEXTUAL_MENU_ID
{
	CONTEXTUAL_TREE_MENU_START = MINID,
	RMENU_DISPLAY_SUBMENU,
	RMENU_CRYPT_SUBMENU,
	RMENU_SHOW_VME,
	RMENU_SHOW_SUBTREE,
	RMENU_SHOW_SAMETYPE,
	RMENU_HIDE_SUBTREE,
	RMENU_HIDE_SAMETYPE,
	RMENU_ADD_GROUP,
	RMENU_CUT,
	RMENU_COPY,
	RMENU_PASTE,
	RMENU_DELETE,
	RMENU_REPARENT,
	RMENU_SORT_TREE,
	RMENU_AUTO_SORT,
	RMENU_CRYPT_VME,
	RMENU_ENABLE_CRYPT_SUBTREE,
	RMENU_DISABLE_CRYPT_SUBTREE,
	CONTEXTUAL_TREE_MENU_STOP
};

//----------------------------------------------------------------------------
// mafGUITreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa mafGUIContextualMenu
*/
class MAF_EXPORT mafGUITreeContextualMenu : public wxMenu, public mafServiceClient
{

public:
  mafGUITreeContextualMenu();
  virtual ~mafGUITreeContextualMenu();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Create a contextual menu*/
  virtual void CreateContextualMenu(mafGUICheckTree *tree, mafView *view, mafVME *vme, bool vme_menu);

  /** Visualize contextual men for the MDI child and selected view. */
  void ShowContextualMenu();

	void SetOpManager(mafOpManager *opManager);

protected:
  mafView       *m_ViewActive;
  mafVME        *m_VmeActive;
  mafVME       *m_NodeActive;
  mafGUICheckTree  *m_NodeTree;
  mafObserver   *m_Listener;

	mafOpManager	*m_OpManager;

  mafSceneGraph *m_SceneGraph;

	wxMenu *m_DisplaySubMenu;

  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	/** Answer contextual men's selection. */
	virtual void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
