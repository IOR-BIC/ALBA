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
#include "mafNode.h"
#include "mafNodeIterator.h"
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
		DeleteNodeList(n->m_Next);
		delete n;
  }
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeAdd(mafNode *vme)   
//----------------------------------------------------------------------------
{
	mafSceneNode *node = NodeAdd(vme);
	if(!node) return;
	
  // must be after NodeAdd
  int nodestatus = m_View->GetNodeStatus(vme);
  node->SetPipeCreatable( nodestatus != NODE_NON_VISIBLE );
  node->m_Mutex         = ( nodestatus == NODE_MUTEX_ON  || 
                            nodestatus == NODE_MUTEX_OFF );

  /* Paolo: 17/3/06
  if (vme->IsA("mafVMEGizmo"))
  {
    VmeShow(vme,true);
  }
  */

  /* @@@

  mafNodeBaseTypes type = mafGetBaseType(vme);

	// initialize node flags - 
	node->m_PipeCreatable = m_creatable[type];
  node->m_Mutex          = m_Mutex    [type];

	// Gizmos are not visible in the DisplayList
	// Gizmo's Pipe is created immediately as a mafPipeGizmo
	if(vme->IsA("mafNodeGizmo")) node->m_Pipe = new mafPipeGizmo(node);

  // Support to LM Handling
  // if the cloud is visible, the LM must be shown automatically
	if(vme->IsA("mafNodeLandmark"))
	{
     mafSceneNode *parentnode = Vme2Node(vme->GetParent());
     VmeShow(vme,parentnode->IsVisible());
	}

	// VmeAdd Root happen only when a new msf is created or loaded
	// reset the camera considering 4dBounds
	
	// Avoid Reset The Camera when there are no children -> returned bounds is invalid 
	if(vme->IsA("mafNodeRoot") && vme->GetNumberOfChildren() && m_RenFront != NULL) //modified by Vladik. 03-03-2004
	{
		float b[6];
		mflBounds bounds;
		vme->GetSpaceBounds(bounds);
		bounds.CopyTo(b);
		m_RenFront->ResetCamera(b);
		m_RenFront->GetActiveCamera()->SetClippingRange(0.1,10000);
		m_View->CameraUpdate();
	}

  // Handle autoshow.
	// Now autoshow work also for mutex vme.
	// Autoshow for mutex vme work only if no other vme of the sme type are already shown.
	if( m_autoshow[type] && node->m_PipeCreatable && !m_shown_mutex_vme[type] )
    mafEventMacro(mafEvent(this, VME_SHOW, vme, true)); 

  @@@ */
}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::NodeAdd(mafNode *vme)   
//----------------------------------------------------------------------------
{
  assert(vme);

  // check if node already exist
	if( Vme2Node(vme) != NULL ) return NULL;

  // find parent node
  mafSceneNode *parent = NULL;
  if( vme->IsA("mafNodeRoot") || vme->IsA("mafVMERoot"))   //SIL. 20-4-2005:  - IsA("mafRoot") si puo fare ?
  {
  }
  else
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
    for(n = m_List; n->m_Next; n=n->m_Next); // go to the end of the list
      n->m_Next = node;
  }
  return node;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeRemove(mafNode *vme)   
//----------------------------------------------------------------------------
{
  VmeShow(vme,false);

  mafSceneNode *node=NULL;

  if(!m_List) return;

  if(m_List->m_Vme == vme)
  {
     node = m_List;
     m_List = m_List->m_Next;
  }
  else
  {
    for(mafSceneNode *n = m_List; n->m_Next; n=n->m_Next) // find prec of node(vme)
    {
      if(n->m_Next->m_Vme == vme)
      {
        node = n->m_Next;
        n->m_Next = n->m_Next->m_Next;
				break; 
      }
    }
  }
	if(node)
		delete node;
}
//----------------------------------------------------------------------------
mafSceneNode *mafSceneGraph::Vme2Node(mafNode *vme)   
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->m_Next)
    if(n->m_Vme == vme)
      return n;
  return NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeSelect(mafNode *vme, bool select)   
//----------------------------------------------------------------------------
{
  if(select) m_SelectedVme = vme;

	mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
  node->Select(select);
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeUpdateProperty(mafNode *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  mafSceneNode *node = Vme2Node(vme);
	if(node == NULL) return;
	node->UpdateProperty(fromTag);
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneGraph::SetCreatableFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
  m_creatable[type] = flag;
}
//----------------------------------------------------------------------------
void mafSceneGraph::SetMutexFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
  m_Mutex[type] = flag;
	if(flag) m_creatable[type] = true;
}
//----------------------------------------------------------------------------
void mafSceneGraph::SetAutoShowFlag(mafNodeBaseTypes type,  bool flag)
//----------------------------------------------------------------------------
{
	if(m_creatable[type])	m_autoshow[type] = flag;
}
@@@ */
//----------------------------------------------------------------------------
void mafSceneGraph::VmeShow(mafNode *vme, bool show)
//----------------------------------------------------------------------------
{
	mafSceneNode *node = Vme2Node(vme);
	if( node == NULL) return;
  
  if(!node->GetPipeCreatable()) return;
  if( node->IsVisible() == show) return;

	if(show)
	{	
    if(node->m_Mutex)
		{
      // Changed code below to allow all MUTEX VMEs to behave at the same manner also if 
      // they are of different type: only one MUTEX VME per time is visible into the view.
      for(mafSceneNode *n = m_List; n; n=n->m_Next)
      {
        //if(n->m_Pipe != NULL && n->m_Vme != vme && n->m_Mutex)
		    mafVME *in_vme = mafVME::SafeDownCast(vme);
		    mafVME *current_vme = mafVME::SafeDownCast(n->m_Vme);
        bool vme_type_check = in_vme != NULL && current_vme != NULL;
        if(n->GetPipe() != NULL && n->m_Vme != vme && vme_type_check && !current_vme->IsA("mafVMEGizmo") && in_vme->GetOutput()->GetTypeId() == current_vme->GetOutput()->GetTypeId())
        {
          mafEventMacro(mafEvent(this,VME_SHOW,n->m_Vme,false));
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
void mafSceneGraph::VmeShowByType(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
  for(mafSceneNode *n = m_List; n; n=n->m_Next)
    if(vme->GetTypeId() == n->m_Vme->GetTypeId() && n->IsVisible() != show)
    {
      //- mutex vme may be shown only is no other vme of the same type is currently shown.
      //- mutex vme may always be hidden.
      mafEventMacro(mafEvent(this,VME_SHOW,n->m_Vme,show));
      if (n->m_Mutex)
      {
        break;
      }
    }
}
//----------------------------------------------------------------------------
// Scan vme subtree and send a VME_SHOW event for each Creatable/non_mutex vme found.
// Changed behavior.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowSubTree(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = vme->NewIterator();
	for(mafNode *v = iter->GetFirstNode(); v; v = iter->GetNextNode())
	{
    mafSceneNode *n = Vme2Node(v);
		if(n && n->GetPipeCreatable() && n->IsVisible() != show )
		{
			// Mutex vme may be shown only is no other vme of the same type is currently shown.
			// Mutex vme may always be hidden.
			if(!show || !n->m_Mutex)
        mafEventMacro(mafEvent(this, VME_SHOW, v, show));
		} 
	}
	iter->Delete();
}


//----------------------------------------------------------------------------
int mafSceneGraph::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  if (node->IsMAFType(mafVMERoot))
  {
    return NODE_NON_VISIBLE;
  }

  mafSceneNode *n = Vme2Node(node);
	if(!n)
    return NODE_NON_VISIBLE;

  if (!node->IsMAFType(mafVME))
  {
    return NODE_NON_VISIBLE;
  }
  mafVME *vme = (mafVME *)node;
  bool creatable = n->GetPipeCreatable() && vme && !vme->GetVisualPipe().IsEmpty();
	//landmark are not creatable
	//if(vme->IsA("mafNodeLandmark")) creatable = false;
  
	if(!creatable)											return NODE_NON_VISIBLE;
  if( n->m_Mutex &&  n->IsVisible())  return NODE_MUTEX_ON;
  if( n->m_Mutex && !n->IsVisible())  return NODE_MUTEX_OFF;
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
      currentNode = currentNode->m_Next;
    }
  }
  else
  {
    os << indent << "The scene graph is empty" << std::endl;
  }
}
