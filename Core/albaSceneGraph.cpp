/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSceneGraph
 Authors: Silvano Imboden
 
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


#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaView.h"
//#include "albaPipeGizmo.h"
//#include "albaPipePointSet.h"
#include "albaGUI.h"
#include "albaIndent.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
albaSceneGraph::albaSceneGraph(albaView *view, vtkRenderer *ren1, vtkRenderer *ren2, vtkRenderer *ren3)
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
albaSceneGraph::~albaSceneGraph( ) 
//----------------------------------------------------------------------------
{
  DeleteNodeList(m_List);
  m_List = NULL;
}
//----------------------------------------------------------------------------
void albaSceneGraph::DeleteNodeList(albaSceneNode *n) 
//----------------------------------------------------------------------------
{
  if(n)
	{
		DeleteNodeList(n->GetNext());
		delete n;
  }
}
//----------------------------------------------------------------------------
void albaSceneGraph::VmeAdd(albaVME *vme)
//----------------------------------------------------------------------------
{
	albaSceneNode *node = NodeAdd(vme);
	if(!node) return;
	
  // must be after NodeAdd
  int nodestatus = m_View->GetNodeStatus(vme);
  node->SetPipeCreatable( nodestatus != NODE_NON_VISIBLE );
  node->SetMutex(nodestatus == NODE_MUTEX_ON  || nodestatus == NODE_MUTEX_OFF );

}
//----------------------------------------------------------------------------
albaSceneNode *albaSceneGraph::NodeAdd(albaVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);

  // check if node already exist
	if( Vme2Node(vme) != NULL ) return NULL;

  // find parent node
  albaSceneNode *parent = NULL;
  if( !vme->IsA("albaVMERoot"))
  {
    parent = Vme2Node(vme->GetParent()); 
    assert(parent);
  }
  
	// create node
  albaSceneNode *node = new albaSceneNode(this,parent,vme,m_RenFront,m_RenBack,m_AlwaysVisibleRenderer) ;

	// insert node in the list
	if(!m_List)
		m_List = node;
  else
  {
    albaSceneNode* n;
    for(n = m_List; n->GetNext(); n=n->GetNext()); // go to the end of the list
      n->SetNext(node);
  }
  return node;
}
//----------------------------------------------------------------------------
void albaSceneGraph::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  VmeShow(vme,false);
	
	if (vme == m_SelectedVme)
		m_SelectedVme = NULL;

  albaSceneNode *node=NULL;

  if(!m_List) return;

  if(m_List->GetVme() == vme)
  {
     node = m_List;
     m_List = m_List->GetNext();
  }
  else
  {
    for(albaSceneNode *n = m_List; n->GetNext(); n=n->GetNext()) // find prec of node(vme)
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
albaSceneNode *albaSceneGraph::Vme2Node(albaVME *vme)
//----------------------------------------------------------------------------
{
  for(albaSceneNode *n = m_List; n; n=n->GetNext())
    if(n->GetVme() == vme)
      return n;
  return NULL;
}
//----------------------------------------------------------------------------
void albaSceneGraph::VmeSelect(albaVME *vme, bool select)
//----------------------------------------------------------------------------
{
  if(select) m_SelectedVme = vme;

	albaSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
  node->Select(select);
}
//----------------------------------------------------------------------------
void albaSceneGraph::VmeUpdateProperty(albaVME *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  albaSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
	node->UpdateProperty(fromTag);
}

//----------------------------------------------------------------------------
void albaSceneGraph::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	albaSceneNode *node = Vme2Node(vme);
	if( node == NULL) return;
  
  if(!node->GetPipeCreatable()) return;
  if( node->IsVisible() == show) return;

	if(show)
	{	
    if(node->GetMutex())
		{
      // Changed code below to allow all MUTEX VMEs to behave at the same manner also if 
      // they are of different type: only one MUTEX VME per time is visible into the view.
      for(albaSceneNode *n = m_List; n; n=n->GetNext())
      {
		    albaVME *current_vme = n->GetVme();
        bool vme_type_check = vme != NULL && current_vme != NULL;
        if(n->GetPipe() != NULL && current_vme != vme && vme_type_check && !current_vme->IsA("albaVMEGizmo") && vme->GetOutput()->GetTypeId() == current_vme->GetOutput()->GetTypeId())
        {
					m_View->VmeShow(n->GetVme(), false);
					break;
        }
      }
		}
    
		if(!node->GetPipe()) 
		{
			m_View->VmeCreatePipe(vme);
      if (!vme->IsALBAType(albaVMEGizmo))
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
void albaSceneGraph::VmeShowByType(albaVME *vme,  bool show)
//----------------------------------------------------------------------------
{
  for(albaSceneNode *n = m_List; n; n=n->GetNext())
    if(vme->GetTypeId() == n->GetVme()->GetTypeId() && n->IsVisible() != show)
    {
      //- mutex vme may be shown only is no other vme of the same type is currently shown.
      //- mutex vme may always be hidden.
			m_View->VmeShow(n->GetVme(), show);
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
void albaSceneGraph::VmeShowSubTree(albaVME *vme,  bool show)
//----------------------------------------------------------------------------
{
  albaVMEIterator *iter = vme->NewIterator();
	for(albaVME *v = iter->GetFirstNode(); v; v = iter->GetNextNode())
	{
		if (v)
		{
			albaSceneNode *n = Vme2Node(v);
			if (n && n->GetPipeCreatable() && n->IsVisible() != show)
			{
				// Mutex vme may be shown only is no other vme of the same type is currently shown.
				// Mutex vme may always be hidden.
				if (!show || !n->GetMutex())
					m_View->VmeShow(v, show);
			}
		}
	}
	iter->Delete();
}


//----------------------------------------------------------------------------
int albaSceneGraph::GetNodeStatus(albaVME *node)
//----------------------------------------------------------------------------
{
  if (node->IsALBAType(albaVMERoot))
  {
    return NODE_NON_VISIBLE;
  }

  albaSceneNode *n = Vme2Node(node);
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
void albaSceneGraph::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  // Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  os << indent << "albaSceneGraph:" << '\t' << this << "\n";

  // print the scene graph 
  albaSceneNode *currentNode = NULL ;

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
