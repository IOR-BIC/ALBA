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



#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUITreeContextualMenu.h" 

#include "wx/utils.h"
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaView.h"
#include "albaViewVTK.h"
#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaGUICheckTree.h"

//#include "albaGUIBitmaps.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
//#include "albaPipe.h"

#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaOpSelect.h"
#include "albaGUI.h"

// Icons
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_RENAME.xpm"
#include "pic/menu/EDIT_REPARENT.xpm"
#include "pic/menu/EDIT_SHOW_HISTORY.xpm"
#include "pic/menu/EDIT_EXPAND_TREE.xpm"
#include "pic/menu/EDIT_COLLAPSE_TREE.xpm"
#include "pic/menu/EDIT_ADD_GROUP.xpm"
#include "pic/menu/SHOW.xpm"
#include "pic/menu/HIDE.xpm"


//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUITreeContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_TREE_MENU_START, CONTEXTUAL_TREE_MENU_STOP, albaGUITreeContextualMenu::OnContextualMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUITreeContextualMenu::albaGUITreeContextualMenu()
:wxMenu()
//----------------------------------------------------------------------------
{
	m_ViewActive  = NULL;
  m_VmeActive   = NULL;
  m_NodeActive  = NULL;
  m_Listener    = NULL;
  m_NodeTree    = NULL;
  m_SceneGraph  = NULL;

  m_Autosort    = false;
  m_CryptoCheck = false;

	m_DisplaySubMenu = NULL;
}
//----------------------------------------------------------------------------
albaGUITreeContextualMenu::~albaGUITreeContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::CreateContextualMenu(albaGUICheckTree *tree, albaView *view, albaVME *vme, bool vme_menu)
//----------------------------------------------------------------------------
{
  m_ViewActive  = view;
  m_NodeActive  = vme;
  m_VmeActive   = vme;

	if (vme_menu)
	{
		bool enable;

		if (m_ViewActive != NULL && (albaViewVTK::SafeDownCast(m_ViewActive) || albaViewCompound::SafeDownCast(m_ViewActive)))
		{
			//albaSceneGraph *sg = NULL;

			int numOfChild = 0;
			if (albaViewCompound::SafeDownCast(m_ViewActive)) {
				numOfChild = ((albaViewCompound *)m_ViewActive)->GetNumberOfSubView();
			}

			if (numOfChild != 0)
			{
				for (int i = 0; i < numOfChild && m_SceneGraph == NULL; i++)
				{
					if (albaViewCompound::SafeDownCast(m_ViewActive))
					{
						m_SceneGraph = ((albaViewCompound *)m_ViewActive)->GetSubView(i)->GetSceneGraph();
					}
				}
			}
			else
			{
				m_SceneGraph = ((albaViewVTK *)m_ViewActive)->GetSceneGraph();
			}

			if (m_SceneGraph != NULL)
			{
				albaSceneNode *n = m_SceneGraph->Vme2Node(m_VmeActive);

				bool show = true;
				if (n)
				{
					show = !m_SceneGraph->Vme2Node(m_VmeActive)->IsVisible();
				}

				m_DisplaySubMenu = new wxMenu();

				if (show)
					albaGUI::AddMenuItem(m_DisplaySubMenu, RMENU_SHOW_VME, "Show", SHOW_xpm);
				else
					albaGUI::AddMenuItem(m_DisplaySubMenu, RMENU_SHOW_VME, "Hide", HIDE_xpm);

				//m_DisplaySubMenu->Append(RMENU_SHOW_VME, text, "");
				m_DisplaySubMenu->AppendSeparator();
				m_DisplaySubMenu->Append(RMENU_SHOW_SUBTREE, "Show sub-tree");
				m_DisplaySubMenu->Append(RMENU_SHOW_SAMETYPE, "Show same type");
				m_DisplaySubMenu->AppendSeparator();
				m_DisplaySubMenu->Append(RMENU_HIDE_SUBTREE, "Hide sub-tree");
				m_DisplaySubMenu->Append(RMENU_HIDE_SAMETYPE, "Hide same type");

				// check visibility 
				m_DisplaySubMenu->FindItem(RMENU_SHOW_VME)->Enable(n->GetPipeCreatable());

				// enable show/hide subtree 
				enable = (m_VmeActive->GetNumberOfChildren() > 0);
				m_DisplaySubMenu->FindItem(RMENU_SHOW_SUBTREE)->Enable(enable);
				m_DisplaySubMenu->FindItem(RMENU_HIDE_SUBTREE)->Enable(enable);

				// enable show/hide same type - must be visualized and not mutex
				enable = n->GetPipeCreatable() && !n->GetMutex();
				m_DisplaySubMenu->FindItem(RMENU_SHOW_SAMETYPE)->Enable(enable);
				m_DisplaySubMenu->FindItem(RMENU_HIDE_SAMETYPE)->Enable(enable);

				this->Append(RMENU_DISPLAY_SUBMENU, _("Display"), m_DisplaySubMenu, wxT(""));
				this->AppendSeparator();
			}
		}

		//////////////////////////////////////////////////////////////////////////
				
		m_NodeTree = tree;
		if (m_NodeTree != NULL && m_VmeActive->GetNumberOfChildren() > 0)
		{
			if (m_VmeActive && m_VmeActive->Equals(m_VmeActive->GetRoot()))
			{
				albaGUI::AddMenuItem(this, RMENU_COLLAPSE_SUBTREE, "Collapse All", EDIT_COLLAPSE_TREE_xpm);
				albaGUI::AddMenuItem(this, RMENU_EXPAND_SUBTREE, "Expand All", EDIT_EXPAND_TREE_xpm);
				this->AppendSeparator();
			}
			else
			{
				if (m_NodeTree->IsNodeExpanded((long long)m_NodeActive))
					albaGUI::AddMenuItem(this, RMENU_COLLAPSE_SUBTREE, "Collapse", EDIT_COLLAPSE_TREE_xpm);
				else
					albaGUI::AddMenuItem(this, RMENU_EXPAND_SUBTREE, "Expand", EDIT_EXPAND_TREE_xpm);

				this->AppendSeparator();
			}
		}

		albaGUI::AddMenuItem(this, RMENU_ADD_GROUP, "Add Group", EDIT_ADD_GROUP_xpm);
		this->AppendSeparator();
		albaGUI::AddMenuItem(this, RMENU_CUT, "Cut", EDIT_CUT_xpm);
		albaGUI::AddMenuItem(this, RMENU_COPY, "Copy", EDIT_COPY_xpm);
		albaGUI::AddMenuItem(this, RMENU_PASTE, "Paste", EDIT_PASTE_xpm);
		albaGUI::AddMenuItem(this, RMENU_DELETE, "Delete", EDIT_DELETE_xpm);
		albaGUI::AddMenuItem(this, RMENU_RENAME, "Rename", EDIT_RENAME_xpm);
		albaGUI::AddMenuItem(this, RMENU_REPARENT, "Reparent to...", EDIT_REPARENT_xpm);
		albaGUI::AddMenuItem(this, RMENU_SHOW_HISTORY, "History", EDIT_SHOW_HISTORY_xpm);

		m_OpManager->EnableContextualMenu(this, m_NodeActive);

		//////////////////////////////////////////////////////////////////////////
		this->AppendSeparator();
		this->Append(RMENU_SORT_TREE, "Sort children nodes");

		if (m_NodeTree != NULL)
		{
			// m_NodeTree == NULL should be only in test mode.
			m_Autosort = m_NodeTree->GetAutoSort();

			this->Append(RMENU_AUTO_SORT, "Keep tree nodes sorted", "", true);
			this->FindItem(RMENU_AUTO_SORT)->Check(m_Autosort);
		}

		//////////////////////////////////////////////////////////////////////////
		this->AppendSeparator();

		wxMenu *cryptSubMenu = new wxMenu();
		cryptSubMenu->Append(RMENU_CRYPT_VME, "Crypt VME", "", true);
		cryptSubMenu->Append(RMENU_ENABLE_CRYPT_SUBTREE, "Enable crypt sub-tree");
		cryptSubMenu->Append(RMENU_DISABLE_CRYPT_SUBTREE, "Disable crypt sub-tree");
		this->Append(RMENU_CRYPT_SUBMENU, _("Crypt"), cryptSubMenu, wxT(""));

		if (m_VmeActive != NULL)
		{
			m_CryptoCheck = m_VmeActive->GetCrypting() != 0;
		}
		else
		{
			m_CryptoCheck = false;
		}

		this->FindItem(RMENU_CRYPT_VME)->Check(m_CryptoCheck);
		this->FindItem(RMENU_CRYPT_VME)->Enable(m_VmeActive != NULL);
		enable = (m_NodeActive->GetNumberOfChildren() > 0);
		this->FindItem(RMENU_ENABLE_CRYPT_SUBTREE)->Enable(enable);
		this->FindItem(RMENU_DISABLE_CRYPT_SUBTREE)->Enable(enable);
	}
}
//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::ShowContextualMenu()
//----------------------------------------------------------------------------
{
	if (m_NodeTree != NULL)
	{
    int x,y;
    ::wxGetMousePosition(&x, &y);
    m_NodeTree->ScreenToClient(&x, &y);
    m_NodeTree->PopupMenu(this, wxPoint(x, y));
	}
  else
  {
    albaLogMessage(_("Warning!! No tree setted to the contextual menu."));
  }
}

void albaGUITreeContextualMenu::SetOpManager(albaOpManager *opManager)
{
	m_OpManager = opManager;
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::OnContextualMenu(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	/*albaSceneGraph *sg = NULL;
  if(m_ViewActive != NULL && albaViewVTK::SafeDownCast(m_ViewActive))
		sg = ((albaViewVTK *)m_ViewActive)->GetSceneGraph();*/
	
	switch(event.GetId())
	{
	case RMENU_ADD_GROUP:
		m_OpManager->RunOpAddGroup();
		break;

	case RMENU_CUT:
		m_OpManager->RunOpCut();
		break;
	case RMENU_COPY:
		m_OpManager->RunOpCopy();
		break;
	case RMENU_PASTE:
		m_OpManager->RunOpPaste();
		break;
	case RMENU_DELETE:
		m_OpManager->RunOpDelete();
		break;
	case RMENU_RENAME:
		m_OpManager->RunOpRename();
		break;
	case RMENU_REPARENT:
		m_OpManager->RunOpReparentTo();
		break;
	case RMENU_SHOW_HISTORY:
		m_OpManager->RunOpShowHistory();
		break;

	  case RMENU_SHOW_VME:
		{
      albaSceneNode *n = NULL;
      if(m_SceneGraph)
        n = m_SceneGraph->Vme2Node(m_VmeActive);

      bool show = true;
      if(n)
        show = !n->IsVisible();
			
			wxString text = "Show";
			if (!show) text = "Hide";
			m_DisplaySubMenu->FindItem(RMENU_SHOW_VME)->SetText(text);

			GetLogicManager()->VmeShow(m_VmeActive, show);
    }
		break;

		case RMENU_EXPAND_SUBTREE:
			ExpandSubTree(m_NodeActive);
		break;
		case RMENU_COLLAPSE_SUBTREE:
			CollapseSubTree(m_NodeActive);
		break;
		
		case RMENU_SHOW_SUBTREE:
			m_SceneGraph->VmeShowSubTree(m_VmeActive, true);
		break;
		case RMENU_HIDE_SUBTREE:
			m_SceneGraph->VmeShowSubTree(m_VmeActive, false);
		break;
		case RMENU_SHOW_SAMETYPE:
			m_SceneGraph->VmeShowByType(m_VmeActive, true);
		break;
		case RMENU_HIDE_SAMETYPE:
			m_SceneGraph->VmeShowByType(m_VmeActive, false);
		break;
    case RMENU_CRYPT_VME:
    {
      m_CryptoCheck = !m_CryptoCheck;
      m_VmeActive->SetCrypting(m_CryptoCheck);
    }
    break;
    case RMENU_ENABLE_CRYPT_SUBTREE:
      CryptSubTree(true);
    break;
    case RMENU_DISABLE_CRYPT_SUBTREE:
      CryptSubTree(false);
    break;
		case RMENU_SORT_TREE:
      m_NodeTree->SortChildren((long long)m_NodeActive);
		break;
		case RMENU_AUTO_SORT:
 			m_Autosort = !m_Autosort;
      m_NodeTree->SetAutoSort(m_Autosort);
			if(m_Autosort) 
        m_NodeTree->SortChildren();
		break;
		default:
			assert(false);
		break;
	}

  if(m_ViewActive != NULL)
		GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::CryptSubTree(bool crypt)
//----------------------------------------------------------------------------
{
  albaVMEIterator *iter = m_NodeActive->NewIterator();

	for(albaVME *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    if(!v->IsA("albaVME"))
      continue;
    v->SetCrypting(crypt);
	}
	iter->Delete();
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::ExpandSubTree(albaVME *node)
{
	m_NodeTree->ExpandNode((long long)node); // Expand first node

	for (int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		if (node->GetChild(i) && node->GetChild(i)->GetNumberOfChildren() > 0)
			ExpandSubTree(node->GetChild(i));
	}
}

//----------------------------------------------------------------------------
void albaGUITreeContextualMenu::CollapseSubTree(albaVME *node)
{
	if(node != node->GetRoot())
		m_NodeTree->CollapseNode((long long)node); // Collapse first node

	for (int i = 0; i < node->GetNumberOfChildren(); i++)
	{
		if (node->GetChild(i) && node->GetChild(i)->GetNumberOfChildren() > 0)
			CollapseSubTree(node->GetChild(i));
	}
}