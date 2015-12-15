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
#include "mafNode.h"
#include "mafNodeIterator.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
enum TREE_CONTEXTUAL_MENU_ID
{
  CONTEXTUAL_TREE_MENU_START = MINID,
		RMENU_SHOW_VME,				
		RMENU_SHOW_SUBTREE,
		RMENU_SHOW_SAMETYPE,
		RMENU_HIDE_SUBTREE,
		RMENU_HIDE_SAMETYPE,
		RMENU_SORT_TREE,
		RMENU_AUTO_SORT,
    RMENU_CRYPT_VME, 
    RMENU_ENABLE_CRYPT_SUBTREE, 
    RMENU_DISABLE_CRYPT_SUBTREE,
	CONTEXTUAL_TREE_MENU_STOP
};

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
}
//----------------------------------------------------------------------------
mafGUITreeContextualMenu::~mafGUITreeContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUITreeContextualMenu::CreateContextualMenu(mafGUICheckTree *tree, mafView *view, mafNode *vme, bool vme_menu)
//----------------------------------------------------------------------------
{
  m_ViewActive  = view;
  m_NodeActive  = vme;
  m_VmeActive   = mafVME::SafeDownCast(vme);

  if(vme_menu)
	{
    bool enable;

    if(m_ViewActive != NULL && (mafViewVTK::SafeDownCast(m_ViewActive) || mafViewCompound::SafeDownCast(m_ViewActive)))
    {
      //mafSceneGraph *sg = NULL;

      int numOfChild = 0;
      if(mafViewCompound::SafeDownCast(m_ViewActive)) {
        numOfChild = ((mafViewCompound *)m_ViewActive)->GetNumberOfSubView();
      }

			if(numOfChild!=0)
			{
				for(int i=0;i<numOfChild && m_SceneGraph==NULL ;i++) 
				{
					if(mafViewCompound::SafeDownCast(m_ViewActive)) 
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

        
        this->Append(RMENU_SHOW_VME, "Hide/Show","");
        this->AppendSeparator();
        this->Append(RMENU_SHOW_SUBTREE,  "Show sub-tree");
        this->Append(RMENU_SHOW_SAMETYPE, "Show same type");
        this->AppendSeparator();
        this->Append(RMENU_HIDE_SUBTREE,  "Hide sub-tree");
        this->Append(RMENU_HIDE_SAMETYPE, "Hide same type");
        this->AppendSeparator();

        // check visibility 
        this->FindItem(RMENU_SHOW_VME)->Enable(n->m_PipeCreatable);

        // enable show/hide subtree 
        enable = (m_VmeActive->GetNumberOfChildren() > 0 );
        this->FindItem(RMENU_SHOW_SUBTREE)->Enable(enable);
        this->FindItem(RMENU_HIDE_SUBTREE)->Enable(enable);

        // enable show/hide same type - must be visualized and not mutex
        enable = n->m_PipeCreatable && !n->m_Mutex;
        this->FindItem(RMENU_SHOW_SAMETYPE)->Enable(enable);
        this->FindItem(RMENU_HIDE_SAMETYPE)->Enable(enable);
      }
    }

	  this->Append(RMENU_CRYPT_VME, "Crypt","",true);
	  this->Append(RMENU_ENABLE_CRYPT_SUBTREE, "Enable crypt sub-tree");
	  this->Append(RMENU_DISABLE_CRYPT_SUBTREE, "Disable crypt sub-tree");
	  this->AppendSeparator();
    this->Append(RMENU_SORT_TREE,		 "Sort children nodes");

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
		enable = (m_NodeActive->GetNumberOfChildren() > 0 );
    this->FindItem(RMENU_ENABLE_CRYPT_SUBTREE)->Enable(enable);
		this->FindItem(RMENU_DISABLE_CRYPT_SUBTREE)->Enable(enable);
	}

  m_NodeTree = tree;
  if (m_NodeTree != NULL)
  {
    // m_NodeTree == NULL should be only in test mode.
	  m_Autosort = m_NodeTree->GetAutoSort();
	
		this->Append(RMENU_AUTO_SORT,	"Keep tree nodes sorted","",true);
		this->FindItem(RMENU_AUTO_SORT)->Check(m_Autosort);
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
//----------------------------------------------------------------------------
void mafGUITreeContextualMenu::OnContextualMenu(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
	/*mafSceneGraph *sg = NULL;
  if(m_ViewActive != NULL && mafViewVTK::SafeDownCast(m_ViewActive))
		sg = ((mafViewVTK *)m_ViewActive)->GetSceneGraph();*/
	
	switch(event.GetId())
	{
	  case RMENU_SHOW_VME:
		{
      mafSceneNode *n = NULL;
      if(m_SceneGraph)
        n = m_SceneGraph->Vme2Node(m_VmeActive);
      bool show = true;
      if(n)
        show = !n->IsVisible();
			mafEventMacro(mafEvent(this, VME_SHOW, m_VmeActive, show));
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
  mafNodeIterator *iter = m_NodeActive->NewIterator();

	for(mafNode *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    if(!v->IsA("mafVME"))
      continue;
    ((mafVME *)v)->SetCrypting(crypt);
	}
	iter->Delete();
}
