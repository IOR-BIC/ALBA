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



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUITreeContextualMenu.h" 

#include "wx/utils.h"
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafView.h"
#include "mafViewVTK.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafGUICheckTree.h"

//#include "mafGUIBitmaps.h"
#include "mafGUILab.h"
#include "mafGUIPicButton.h"
//#include "mafPipe.h"

#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafOpSelect.h"
#include "mafGUI.h"

// Icons
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_REPARENT.xpm"
#include "pic/menu/EDIT_ADD_GROUP.xpm"
#include "pic/menu/SHOW.xpm"
#include "pic/menu/HIDE.xpm"


//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUITreeContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_TREE_MENU_START, CONTEXTUAL_TREE_MENU_STOP, mafGUITreeContextualMenu::OnContextualMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUITreeContextualMenu::mafGUITreeContextualMenu()
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
mafGUITreeContextualMenu::~mafGUITreeContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUITreeContextualMenu::CreateContextualMenu(mafGUICheckTree *tree, mafView *view, mafVME *vme, bool vme_menu)
//----------------------------------------------------------------------------
{
  m_ViewActive  = view;
  m_NodeActive  = vme;
  m_VmeActive   = vme;

	if (vme_menu)
	{
		bool enable;

		if (m_ViewActive != NULL && (mafViewVTK::SafeDownCast(m_ViewActive) || mafViewCompound::SafeDownCast(m_ViewActive)))
		{
			//mafSceneGraph *sg = NULL;

			int numOfChild = 0;
			if (mafViewCompound::SafeDownCast(m_ViewActive)) {
				numOfChild = ((mafViewCompound *)m_ViewActive)->GetNumberOfSubView();
			}

			if (numOfChild != 0)
			{
				for (int i = 0; i < numOfChild && m_SceneGraph == NULL; i++)
				{
					if (mafViewCompound::SafeDownCast(m_ViewActive))
					{
						m_SceneGraph = ((mafViewCompound *)m_ViewActive)->GetSubView(i)->GetSceneGraph();
					}
				}
			}
			else
			{
				m_SceneGraph = ((mafViewVTK *)m_ViewActive)->GetSceneGraph();
			}

			if (m_SceneGraph != NULL)
			{
				mafSceneNode *n = m_SceneGraph->Vme2Node(m_VmeActive);

				bool show = true;
				if (n)
				{
					show = !m_SceneGraph->Vme2Node(m_VmeActive)->IsVisible();
				}

				m_DisplaySubMenu = new wxMenu();

				if (show)
					mafGUI::AddMenuItem(m_DisplaySubMenu, RMENU_SHOW_VME, "Show", SHOW_xpm);
				else
					mafGUI::AddMenuItem(m_DisplaySubMenu, RMENU_SHOW_VME, "Hide", HIDE_xpm);

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

		mafGUI::AddMenuItem(this, RMENU_ADD_GROUP, "Add Group", EDIT_ADD_GROUP_xpm);
		this->AppendSeparator();
		mafGUI::AddMenuItem(this, RMENU_CUT, "Cut", EDIT_CUT_xpm);
		mafGUI::AddMenuItem(this, RMENU_COPY, "Copy", EDIT_COPY_xpm);
		mafGUI::AddMenuItem(this, RMENU_PASTE, "Paste", EDIT_PASTE_xpm);
		mafGUI::AddMenuItem(this, RMENU_DELETE, "Delete", EDIT_DELETE_xpm);
		mafGUI::AddMenuItem(this, RMENU_REPARENT, "Reparent to...", EDIT_REPARENT_xpm);

		m_OpManager->EnableContextualMenu(this, m_NodeActive);

		//////////////////////////////////////////////////////////////////////////
		this->AppendSeparator();
		this->Append(RMENU_SORT_TREE, "Sort children nodes");

		m_NodeTree = tree;
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
void mafGUITreeContextualMenu::ShowContextualMenu()
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
    mafLogMessage(_("Warning!! No tree setted to the contextual menu."));
  }
}

void mafGUITreeContextualMenu::SetOpManager(mafOpManager *opManager)
{
	m_OpManager = opManager;
}

//----------------------------------------------------------------------------
void mafGUITreeContextualMenu::OnContextualMenu(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	/*mafSceneGraph *sg = NULL;
  if(m_ViewActive != NULL && mafViewVTK::SafeDownCast(m_ViewActive))
		sg = ((mafViewVTK *)m_ViewActive)->GetSceneGraph();*/
	
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
	case RMENU_REPARENT:
		m_OpManager->RunOpReparentTo();
		break;;

	  case RMENU_SHOW_VME:
		{
      mafSceneNode *n = NULL;
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
      m_NodeTree->SortChildren((long)m_NodeActive);
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
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafGUITreeContextualMenu::CryptSubTree(bool crypt)
//----------------------------------------------------------------------------
{
  mafVMEIterator *iter = m_NodeActive->NewIterator();

	for(mafVME *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    if(!v->IsA("mafVME"))
      continue;
    v->SetCrypting(crypt);
	}
	iter->Delete();
}
