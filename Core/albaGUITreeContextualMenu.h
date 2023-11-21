#include "albaOpManager.h"
/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITreeContextualMenu
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaGUITreeContextualMenu_H__
#define __albaGUITreeContextualMenu_H__

#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class albaView;
class albaGUIMDIChild;
class albaEvent;
class albaObserver;
class albaGUICheckTree;
class albaVME;
class albaSceneGraph;
class albaVME;
class albaOpManager;

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
	RMENU_EXPAND_SUBTREE,
	RMENU_COLLAPSE_SUBTREE,
	RMENU_ADD_GROUP,
	RMENU_CUT,
	RMENU_COPY,
	RMENU_PASTE,
	RMENU_DELETE,
	RMENU_RENAME,
	RMENU_REPARENT,
	RMENU_SHOW_HISTORY,
	RMENU_SORT_TREE,
	RMENU_AUTO_SORT,
	RMENU_CRYPT_VME,
	RMENU_ENABLE_CRYPT_SUBTREE,
	RMENU_DISABLE_CRYPT_SUBTREE,
	CONTEXTUAL_TREE_MENU_STOP
};

//----------------------------------------------------------------------------
// albaGUITreeContextualMenu :
//----------------------------------------------------------------------------
/** Used to create and manage contextual menu' for tree widget used in side bar
@sa albaGUIContextualMenu
*/
class ALBA_EXPORT albaGUITreeContextualMenu : public wxMenu, public albaServiceClient
{

public:
  albaGUITreeContextualMenu();
  virtual ~albaGUITreeContextualMenu();
  void SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Create a contextual menu*/
  virtual void CreateContextualMenu(albaGUICheckTree *tree, albaView *view, albaVME *vme, bool vme_menu);

  /** Visualize contextual men for the MDI child and selected view. */
  void ShowContextualMenu();

	void SetOpManager(albaOpManager *opManager);

protected:
  albaView       *m_ViewActive;
  albaVME        *m_VmeActive;
  albaVME       *m_NodeActive;
  albaGUICheckTree  *m_NodeTree;
  albaObserver   *m_Listener;

	albaOpManager	*m_OpManager;

  albaSceneGraph *m_SceneGraph;

	wxMenu *m_DisplaySubMenu;

  bool m_Autosort;
  bool m_CryptoCheck;

  /** Enable/disable crypto for a subtree. */
  void CryptSubTree(bool crypt);
  
	void ExpandSubTree(albaVME *vme);
	void CollapseSubTree(albaVME *vme);

	/** Answer contextual men's selection. */
	virtual void OnContextualMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
