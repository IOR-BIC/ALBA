/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTreeContextualMenu.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 15:26:09 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
#include "mmgTreeContextualMenu.h" 

#include "wx/utils.h"
#include <wx/laywin.h>
#include <wx/mdi.h>

#include "mafDecl.h"
#include "mafEvent.h"
//#include "mafVmeData.h"
#include "mafView.h"
//#include "mafSceneNode.h"
//#include "mafSceneGraph.h"
#include "mmgCheckTree.h"

//#include "mmgBitmaps.h"
#include "mmgLab.h"
#include "mmgPicButton.h"
//#include "mafPipe.h"

//#include "mflAssembly.h"
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
BEGIN_EVENT_TABLE(mmgTreeContextualMenu,wxMenu)
  EVT_MENU_RANGE(CONTEXTUAL_TREE_MENU_START, CONTEXTUAL_TREE_MENU_STOP, mmgTreeContextualMenu::OnContextualMenu)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgTreeContextualMenu::mmgTreeContextualMenu()
:wxMenu()
//----------------------------------------------------------------------------
{
	m_view        = NULL;
  m_clicked_vme = NULL;
  m_clicked_node= NULL;
  m_Listener    = NULL;
  m_tree        = NULL;

  m_autosort = false;
  m_check_crypto = false;
}
//----------------------------------------------------------------------------
mmgTreeContextualMenu::~mmgTreeContextualMenu()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgTreeContextualMenu::ShowContextualMenu(mmgCheckTree *tree, mafView *view, mafNode *vme, bool vme_menu)
//----------------------------------------------------------------------------
{
  m_view        = view;
  m_clicked_node= vme;
  m_clicked_vme = mafVME::SafeDownCast(vme);

  m_tree        = tree;
  m_autosort    = m_tree->GetAutoSort();
  
  if(vme_menu)
	{
    bool enable;

    if(m_view != NULL)
    {
/*      mafSceneGraph *sg = NULL;
	    sg = m_view->GetSceneGraph(); 
      mafSceneNode *n = sg->Vme2Node(m_clicked_vme);

      this->Append(RMENU_SHOW_VME, "Hide/Show","");
		  this->AppendSeparator();
		  this->Append(RMENU_SHOW_SUBTREE,	 "Show sub-tree");
		  this->Append(RMENU_SHOW_SAMETYPE, "Show same type");
		  this->AppendSeparator();
		  this->Append(RMENU_HIDE_SUBTREE,  "Hide sub-tree");
		  this->Append(RMENU_HIDE_SAMETYPE, "Hide same type");
		  this->AppendSeparator();

		  // check visibility 
		  this->FindItem(RMENU_SHOW_VME)->Enable(n->m_pipe_creatable);  
		  
		  // enable show/hide subtree 
      enable = (m_clicked_vme->GetNumberOfChildren() > 0 );
		  this->FindItem(RMENU_SHOW_SUBTREE)->Enable(enable);
		  this->FindItem(RMENU_HIDE_SUBTREE)->Enable(enable);

		  // enable show/hide same type - must be visualized and not mutex
		  enable = n->m_pipe_creatable && !n->m_mutex;
		  this->FindItem(RMENU_SHOW_SAMETYPE)->Enable(enable);
		  this->FindItem(RMENU_HIDE_SAMETYPE)->Enable(enable);*/
    }

	  this->Append(RMENU_CRYPT_VME, "Crypt","",true);
	  this->Append(RMENU_ENABLE_CRYPT_SUBTREE, "Enable crypt sub-tree");
	  this->Append(RMENU_DISABLE_CRYPT_SUBTREE, "Disable crypt sub-tree");
	  this->AppendSeparator();
    this->Append(RMENU_SORT_TREE,		 "Sort children nodes");

    if (m_clicked_vme != NULL)
    {
      m_check_crypto = m_clicked_vme->GetCrypting() != 0;
    }
    else
    {
      m_check_crypto = false;
    }
    
    this->FindItem(RMENU_CRYPT_VME)->Check(m_check_crypto);
    this->FindItem(RMENU_CRYPT_VME)->Enable(m_clicked_vme != NULL);
		enable = (m_clicked_node->GetNumberOfChildren() > 0 );
    this->FindItem(RMENU_ENABLE_CRYPT_SUBTREE)->Enable(enable);
		this->FindItem(RMENU_DISABLE_CRYPT_SUBTREE)->Enable(enable);
	}

	this->Append(RMENU_AUTO_SORT,		 "Keep tree nodes sorted","",true);
	this->FindItem(RMENU_AUTO_SORT)->Check(m_autosort);

	int x,y;
	::wxGetMousePosition(&x, &y);
  m_tree->ScreenToClient(&x, &y);
	m_tree->PopupMenu(this, wxPoint(x, y));
}
//----------------------------------------------------------------------------
void mmgTreeContextualMenu::OnContextualMenu(wxCommandEvent &event)
//----------------------------------------------------------------------------
{
  /*
	mafSceneGraph *sg = NULL;
  if(m_view != NULL)
		sg = m_view->GetSceneGraph();*/
	
	switch(event.GetId())
	{
	  /*case RMENU_SHOW_VME:
		{
      mafSceneNode *n = NULL;
      if(sg)
        n = sg->Vme2Node(m_clicked_vme);
      bool show = true;
      if(n)
        show = n->m_pipe != NULL;
			mafEventMacro(mafEvent(this, VME_SHOW, m_clicked_vme, !show));
    }
		break;
		case RMENU_SHOW_SUBTREE:
			sg->VmeShowSubTree(m_clicked_vme, true);
		break;
		case RMENU_HIDE_SUBTREE:
			sg->VmeShowSubTree(m_clicked_vme, false);
		break;
		case RMENU_SHOW_SAMETYPE:
			sg->VmeShowByType(m_clicked_vme, true);
		break;
		case RMENU_HIDE_SAMETYPE:
			sg->VmeShowByType(m_clicked_vme, false);
		break;*/
    case RMENU_CRYPT_VME:
    {
      m_check_crypto = !m_check_crypto;
      m_clicked_vme->SetCrypting(m_check_crypto);
      //((mafVmeData *)m_clicked_vme->GetClientData())->UpdateFromTag();
    }
    break;
    case RMENU_ENABLE_CRYPT_SUBTREE:
      CryptSubTree(true);
    break;
    case RMENU_DISABLE_CRYPT_SUBTREE:
      CryptSubTree(false);
    break;
		case RMENU_SORT_TREE:
      m_tree->SortChildren((long)m_clicked_node);
		break;
		case RMENU_AUTO_SORT:
 			m_autosort = !m_autosort;
      m_tree->SetAutoSort(m_autosort);
			if(m_autosort) 
        m_tree->SortChildren();
		break;
		default:
			assert(false);
		break;
	}

  if(m_view != NULL)
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmgTreeContextualMenu::CryptSubTree(bool crypt)
//----------------------------------------------------------------------------
{
//  mafVmeData *vd = NULL;
  mafNodeIterator *iter = m_clicked_node->NewIterator();

	for(mafNode *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    if(!v->IsA("mafVME"))
      continue;
    ((mafVME *)v)->SetCrypting(crypt);
//    vd = (mafVmeData *)v->GetClientData();
//    if (vd == NULL)
//    {
//      mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,v));
//      vd = (mafVmeData *)v->GetClientData();
//    }
//    vd->UpdateFromTag();
	}
	iter->Delete();
}