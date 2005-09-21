/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2005-09-21 12:41:59 $
  Version:   $Revision: 1.11 $
  Authors:   Silvano Imboden
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


#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafView.h"
//#include "mafPipeGizmo.h"
//#include "mafPipePointSet.h"
#include "mmgGui.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMEGizmo.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafSceneGraph::mafSceneGraph(mafView *view, vtkRenderer *ren1, vtkRenderer *ren2)
//----------------------------------------------------------------------------
{
	m_RenFront  = ren1;
	m_RenBack   = ren2;
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
  node->m_PipeCreatable = ( nodestatus != NODE_NON_VISIBLE );
  node->m_Mutex          = ( nodestatus == NODE_MUTEX_ON  || nodestatus == NODE_MUTEX_OFF );

  if (vme->IsA("mafVMEGizmo"))
  {
    VmeShow(vme,true);
  }

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
  mafSceneNode *node = new mafSceneNode(this,parent,vme,m_RenFront,m_RenBack) ;

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

  mafSceneNode *node;

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
  
  /*
  // Support to LM Handling
	if(vme->IsA("mafNodeLandmarkCloud"))
	{
		mafNodeLandmarkCloud *cloud = (mafNodeLandmarkCloud*)vme;
		if(  cloud->IsOpen()  )
    {
   		 // don't create the pipe for the cloud, 
			 // but set the Visible flag anyway
			 node->Show(show);

       // forward the VME_SHOW to the child Landmarks
			 int nc = cloud->GetNumberOfLandmarks();
			 for(int i=0;i<nc;i++)
			 {
				  VmeShow(cloud->GetLandmark(i),show); 
       }
		   return;
		}
  }
  */
  if(!node->m_PipeCreatable) return;
  if( node->IsVisible() == show) return;

	if(vme->IsA("mafVMEGizmo"))
  {
    //nothing
  }
  else
  {
		if(show)
		{	
      /* @@@		
      if(node->m_Mutex)
			{
				// find other visible node with the same type and hide it
				//wxString type = vme->GetClassName();
        int type = mafGetBaseType(vme);
				for( mafSceneNode *n = m_List; n; n=n->m_Next)
					//if(type == n->m_Vme->GetClassName() && n->m_Pipe != NULL )
          if(type == mafGetBaseType(n->m_Vme) && n->m_Pipe != NULL )
					  mafEventMacro(mafEvent(this,VME_SHOW,n->m_Vme,false));
			}
      @@@ */
			if(!node->m_Pipe) 
			{
				m_View->VmeCreatePipe(vme);
				VmeSelect(vme,vme == m_SelectedVme);
			}
		}
		else
		{
			if(node->m_Pipe) m_View->VmeDeletePipe(vme);
		}
  }
	//node->Show(show);  //SIL. 21-4-2005: 
  
  /* @@@
  // update m_shown_mutex_vme
	if(node->m_Mutex)
	{
		mafNodeBaseTypes type = mafGetBaseType(vme);
		m_shown_mutex_vme[type] = (show) ? vme : NULL;
	}
  @@@ */
}
/* @@@
//----------------------------------------------------------------------------
void mafSceneGraph::VmeShowByType(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
	VmeShowByType( mafGetBaseType(vme), show);
}
//----------------------------------------------------------------------------
// Scan the tree and send a VME_SHOW event for each vme with the same type.
// Type should be creatable and non mutex.
// Changed behavoir.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowByType(mafNodeBaseTypes type,	bool show)
//----------------------------------------------------------------------------
{
	if(!m_creatable[type] ) return;
	
	for( mafSceneNode *n = m_List; n; n=n->m_Next)
	  if(type == mafGetBaseType(n->m_Vme) && n->IsVisible() != show )
			//- mutex vme may be shown only is no other vme of the same type is currently shown.
			//- mutex vme may always be hidden.
			if(!show || m_shown_mutex_vme[type]==NULL) 
				mafEventMacro(mafEvent(this,VME_SHOW,n->m_Vme,show));
}
@@@ */

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
		if(n && n->m_PipeCreatable && n->IsVisible() != show )
		{
			// Mutex vme may be shown only is no other vme of the same type is currently shown.
			// Mutex vme may always be hidden.
			if(!show || !n->m_Mutex)
        mafEventMacro(mafEvent(this, VME_SHOW, v, show));
		} 
	}
	iter->Delete();
}

/* @@@
//----------------------------------------------------------------------------
// gui constants
//----------------------------------------------------------------------------
enum 
{
	ID_AUTOSHOW_SURFACE = MINID,
	ID_AUTOSHOW_VOLUME,
  ID_AUTOSHOW_GRAY_VOLUME,
	ID_AUTOSHOW_IMAGE,
	ID_AUTOSHOW_POINTSET,
	ID_AUTOSHOW_SCALAR,

};
//----------------------------------------------------------------------------
mmgGui *mafSceneGraph::GetGui()
//----------------------------------------------------------------------------
{
	if(m_Gui == NULL)
	{
		m_Gui = new mmgGui(this);
		
		bool b1 = m_creatable[VME_SURFACE]	&& !m_Mutex[VME_SURFACE];
		bool b2 = m_creatable[VME_VOLUME]		&& !m_Mutex[VME_VOLUME];
		bool b3 = m_creatable[VME_GRAY_VOLUME]		&& !m_Mutex[VME_GRAY_VOLUME];
		bool b4 = m_creatable[VME_IMAGE]		&& !m_Mutex[VME_IMAGE];
		bool b5 = m_creatable[VME_POINTSET] && !m_Mutex[VME_POINTSET];

		//modified by STEFY 30-4-2004(begin)
		bool b6 = m_creatable[VME_SCALAR] && !m_Mutex[VME_SCALAR];
		//modified by STEFY 30-4-2004(end)
		
		//if(b1 || b2 || b3 || b4 || b5) m_Gui->Label("display list",true);
		if(b1 || b2 || b3 || b4 || b5 || b6) m_Gui->Label("display list",true);

		if(b1) m_Gui->Bool(ID_AUTOSHOW_SURFACE,  "auto show all Surfaces",  &m_autoshow[VME_SURFACE], true);
		if(b2) m_Gui->Bool(ID_AUTOSHOW_VOLUME,   "auto show all Volumes",   &m_autoshow[VME_VOLUME],  true);
		if(b3) m_Gui->Bool(ID_AUTOSHOW_GRAY_VOLUME,   "auto show all Gray Volumes",   &m_autoshow[VME_GRAY_VOLUME],  true);
		if(b4) m_Gui->Bool(ID_AUTOSHOW_IMAGE,    "auto show all Images",    &m_autoshow[VME_IMAGE],   true);
		if(b5) m_Gui->Bool(ID_AUTOSHOW_POINTSET, "auto show all Landmarks", &m_autoshow[VME_POINTSET],true);
		
		//modified by STEFY 30-4-2004(begin)
		if(b5) m_Gui->Bool(ID_AUTOSHOW_SCALAR, "auto show all Scalar", &m_autoshow[VME_SCALAR],true);
		//modified by STEFY 30-4-2004(end)

		m_Gui->Update();
	}
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafSceneGraph::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
	switch(e.GetId())
	{
		case ID_AUTOSHOW_SURFACE:
      VmeShowByType(VME_SURFACE,	m_autoshow[VME_SURFACE]!=0);
		break;
		case ID_AUTOSHOW_VOLUME:
      VmeShowByType(VME_VOLUME,	m_autoshow[VME_VOLUME]!=0);
    break;
		case ID_AUTOSHOW_GRAY_VOLUME:
      VmeShowByType(VME_GRAY_VOLUME,	m_autoshow[VME_GRAY_VOLUME]!=0);
    break;
		case ID_AUTOSHOW_IMAGE:
      VmeShowByType(VME_IMAGE,		m_autoshow[VME_IMAGE]!=0);
    break;
		case ID_AUTOSHOW_POINTSET:
      VmeShowByType(VME_POINTSET,m_autoshow[VME_POINTSET]!=0);
    break;
	//modified by STEFY 30-4-2004(begin)
		case ID_AUTOSHOW_SCALAR:
      VmeShowByType(VME_SCALAR,m_autoshow[VME_SCALAR]!=0);
    break;
	//modified by STEFY 30-4-2004(end)
    default:
      mafEventMacro(e);
			return;
    break;
	}
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
@@@ */
/* @@@
//----------------------------------------------------------------------------
void mafSceneGraph::OnOpenCloseEvent(mafSceneNode *node)
//----------------------------------------------------------------------------
{
  assert(node->m_cloud);
	if(node->m_cloud->IsOpen()  && node->m_Pipe)  m_View->VmeDeletePipe(node->m_cloud);
	if(!node->m_cloud->IsOpen() && !node->m_Pipe && node->IsVisible())
	{
		m_View->VmeCreatePipe(node->m_cloud);
		VmeSelect(node->m_cloud,node->m_cloud== m_SelectedVme);
  }
}
@@@ */

//----------------------------------------------------------------------------
int mafSceneGraph::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  if (vme->IsMAFType(mafVMERoot))
  {
    return NODE_NON_VISIBLE;
  }

  mafSceneNode *n = Vme2Node(vme);
	if(!n )
    return NODE_NON_VISIBLE;

  bool creatable = n->m_PipeCreatable;
	//landmark are not creatable
	//if(vme->IsA("mafNodeLandmark")) creatable = false;
  
	if(!creatable)											return NODE_NON_VISIBLE;
  if( n->m_Mutex &&  n->IsVisible())  return NODE_MUTEX_ON;
  if( n->m_Mutex && !n->IsVisible())  return NODE_MUTEX_OFF;
  if( n->IsVisible())									return NODE_VISIBLE_ON;
  if(!n->IsVisible())									return NODE_VISIBLE_OFF;
	return NODE_NON_VISIBLE;
}
