/*=========================================================================

 Program: MAF2
 Module: mafSceneGraph
 Authors: Silvano Imboden
 
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


#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafView.h"
//#include "mafPipeGizmo.h"
//#include "mafPipePointSet.h"
#include "mafGUI.h"
#include "mafIndent.h"
#include "mafVME.h"
#include "mafVMEIterator.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafSceneGraph::mafSceneGraph(mafView *view, vtkRenderer *ren1, vtkRenderer *ren2, vtkRenderer *ren3)
//----------------------------------------------------------------------------
{
	m_RenFront  = ren1;
	m_RenBack   = ren2;
  m_AlwaysVisibleRenderer = ren3;
	m_View      = view;

  m_Gui          = NULL;
  m_Listener     = NULL;
  m_List         = NULL;
	m_SelectedVme  = NULL;

/*
  int i;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_creatable[i] = m_Mutex[i] = false;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_autoshow[i] = 0;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_shown_mutex_vme[i] = NULL;
*/
}
//----------------------------------------------------------------------------
mafSceneGraph::~mafSceneGraph( ) 
//----------------------------------------------------------------------------
{
  DeleteNodeList(m_List);
  m_List = NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::DeleteNodeList(mafSceneNode *n) 
//----------------------------------------------------------------------------
{
  if(n)
	{
		DeleteNodeList(n->GetNext());
		delete n;
  }
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeAdd(mafVME *vme)
//----------------------------------------------------------------------------
{
	mafSceneNode *node = NodeAdd(vme);
	if(!node) return;
	
  // must be after NodeAdd
  int nodestatus = m_View->GetNodeStatus(vme);
  node->SetPipeCreatable( nodestatus != NODE_NON_VISIBLE );
  node->SetMutex(nodestatus == NODE_MUTEX_ON  || nodestatus == NODE_MUTEX_OFF );

}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::NodeAdd(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);

  // check if node already exist
	if( Vme2Node(vme) != NULL ) return NULL;

  // find parent node
  mafSceneNode *parent = NULL;
  if( !vme->IsA("mafVMERoot"))
  {
    parent = Vme2Node(vme->GetParent()); 
    assert(parent);
  }
  
	// create node
  mafSceneNode *node = new mafSceneNode(this,parent,vme,m_RenFront,m_RenBack,m_AlwaysVisibleRenderer) ;

	// insert node in the list
	if(!m_List)
		m_List = node;
  else
  {
    mafSceneNode* n;
    for(n = m_List; n->GetNext(); n=n->GetNext()); // go to the end of the list
      n->SetNext(node);
  }
  return node;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeRemove(mafVME *vme)
//----------------------------------------------------------------------------
{
  VmeShow(vme,false);

  mafSceneNode *node=NULL;

  if(!m_List) return;

  if(m_List->GetVme() == vme)
  {
     node = m_List;
     m_List = m_List->GetNext();
  }
  else
  {
    for(mafSceneNode *n = m_List; n->GetNext(); n=n->GetNext()) // find prec of node(vme)
    {
      if(n->GetNext()->GetVme() == vme)
      {
        node = n->GetNext();
				n->SetNext(n->GetNext()->GetNext());;
				break; 
      }
    }
  }
	if(node)
		delete node;
}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::Vme2Node(mafVME *vme)
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->GetNext())
    if(n->GetVme() == vme)
      return n;
  return NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeSelect(mafVME *vme, bool select)
//----------------------------------------------------------------------------
{
  if(select) m_SelectedVme = vme;

	mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
  node->Select(select);
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeUpdateProperty(mafVME *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
	node->UpdateProperty(fromTag);
}

//----------------------------------------------------------------------------
void mafSceneGraph::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
	mafSceneNode *node = Vme2Node(vme);
	if( node == NULL) return;
  
  if(!node->GetPipeCreatable()) return;
  if( node->IsVisible() == show) return;

	if(show)
	{	
    if(node->GetMutex())
		{
      // Changed code below to allow all MUTEX VMEs to behave at the same manner also if 
      // they are of different type: only one MUTEX VME per time is visible into the view.
      for(mafSceneNode *n = m_List; n; n=n->GetNext())
      {
		    mafVME *current_vme = n->GetVme();
        bool vme_type_check = vme != NULL && current_vme != NULL;
        if(n->GetPipe() != NULL && current_vme != vme && vme_type_check && !current_vme->IsA("mafVMEGizmo") && vme->GetOutput()->GetTypeId() == current_vme->GetOutput()->GetTypeId())
        {
					GetLogicManager()->VmeShow(n->GetVme(), false);
					break;
        }
      }
		}
    
		if(!node->GetPipe()) 
		{
			m_View->VmeCreatePipe(vme);
      if (!vme->IsMAFType(mafVMEGizmo))
      {
        if(m_View->GetNumberOfVisibleVME() == 1)
        {
          m_View->CameraReset();
        }
        else
        {
          m_View->CameraUpdate();
        }
      }
			VmeSelect(vme,vme == m_SelectedVme);
		}
	}
	else //show == false
	{
		if(node->GetPipe()) m_View->VmeDeletePipe(vme);
	}

}

//----------------------------------------------------------------------------
void mafSceneGraph::VmeShowByType(mafVME *vme,  bool show)
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->GetNext())
    if(vme->GetTypeId() == n->GetVme()->GetTypeId() && n->IsVisible() != show)
    {
      //- mutex vme may be shown only is no other vme of the same type is currently shown.
      //- mutex vme may always be hidden.
			GetLogicManager()->VmeShow(n->GetVme(), show);
      if (n->GetMutex())
      {
        break;
      }
    }
}
//----------------------------------------------------------------------------
// Scan vme subtree and Show for each Creatable/non_mutex vme found.
// Changed behavior.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowSubTree(mafVME *vme,  bool show)
//----------------------------------------------------------------------------
{
  mafVMEIterator *iter = vme->NewIterator();
	for(mafVME *v = iter->GetFirstNode(); v; v = iter->GetNextNode())
	{
		if (v)
		{
			mafSceneNode *n = Vme2Node(v);
			if (n && n->GetPipeCreatable() && n->IsVisible() != show)
			{
				// Mutex vme may be shown only is no other vme of the same type is currently shown.
				// Mutex vme may always be hidden.
				if (!show || !n->GetMutex())
					GetLogicManager()->VmeShow(v, show);
			}
		}
	}
	iter->Delete();
}


//----------------------------------------------------------------------------
int mafSceneGraph::GetNodeStatus(mafVME *node)
//----------------------------------------------------------------------------
{
  if (node->IsMAFType(mafVMERoot))
  {
    return NODE_NON_VISIBLE;
  }

  mafSceneNode *n = Vme2Node(node);
	if(!n)
    return NODE_NON_VISIBLE;

  bool creatable = n->GetPipeCreatable() && node && !node->GetVisualPipe().IsEmpty();
  
	if(!creatable)											return NODE_NON_VISIBLE;
  if( n->GetMutex() &&  n->IsVisible())  return NODE_MUTEX_ON;
  if( n->GetMutex() && !n->IsVisible())  return NODE_MUTEX_OFF;
  if( n->IsVisible())									return NODE_VISIBLE_ON;
  if(!n->IsVisible())									return NODE_VISIBLE_OFF;
	return NODE_NON_VISIBLE;
}

//-------------------------------------------------------------------------
void mafSceneGraph::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  // Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "mafSceneGraph:" << '\t' << this << "\n";

  // print the scene graph 
  mafSceneNode *currentNode = NULL ;

  // if there is at least one node...  
  if(m_List)
  {
    currentNode = m_List;

    while (currentNode)
    {
      //print the current node
      currentNode->Print(os, 1);

      //go to the next node
      currentNode = currentNode->GetNext();
    }
  }
  else
  {
    os << indent << "The scene graph is empty" << std::endl;
  }
}
