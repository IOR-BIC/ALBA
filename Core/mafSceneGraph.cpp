/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:37:43 $
  Version:   $Revision: 1.2 $
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


#include "mafSceneNode.h"
#include "mafView.h"
//#include "mafPipeGizmo.h"
//#include "mafPipePointSet.h"
#include "mmgGUI.h"
#include "mafSceneGraph.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
//#include "mflBounds.h"
//#include "mafNodeLandmark.h"
//#include "mafNodeGizmo.h"
//#include "mafNodeLandmarkCloud.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafSceneGraph::mafSceneGraph(mafView *view, vtkRenderer *ren1, vtkRenderer *ren2)
//----------------------------------------------------------------------------
{
	m_ren1     = ren1;
	m_ren2     = ren2;
	m_view     = view;

  m_gui          = NULL;
  m_listener     = NULL;
  m_list         = NULL;
	m_selected_vme = NULL;

/*
  int i;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_creatable[i] = m_mutex[i] = false;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_autoshow[i] = 0;
	for(i = 0; i<NUM_OF_BASETYPE; i++) m_shown_mutex_vme[i] = NULL;
*/
}
//----------------------------------------------------------------------------
mafSceneGraph::~mafSceneGraph( ) 
//----------------------------------------------------------------------------
{
  DeleteNodeList(m_list);
  m_list = NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::DeleteNodeList(mafSceneNode *n) 
//----------------------------------------------------------------------------
{
  if(n)
	{
		DeleteNodeList(n->m_next);
		delete n;
  }
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeAdd(mafNode *vme)   
//----------------------------------------------------------------------------
{
	mafSceneNode *node = NodeAdd(vme);
	if(!node) return;
	
  /* @@@

  mafNodeBaseTypes type = mafGetBaseType(vme);

	// initialize node flags - 
	node->m_pipe_creatable = m_creatable[type];
  node->m_mutex          = m_mutex    [type];

	// Gizmos are not visible in the DisplayList
	// Gizmo's Pipe is created immediately as a mafPipeGizmo
	if(vme->IsA("mafNodeGizmo")) node->m_pipe = new mafPipeGizmo(node);

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
	if(vme->IsA("mafNodeRoot") && vme->GetNumberOfChildren() && m_ren1 != NULL) //modified by Vladik. 03-03-2004
	{
		float b[6];
		mflBounds bounds;
		vme->GetSpaceBounds(bounds);
		bounds.CopyTo(b);
		m_ren1->ResetCamera(b);
		m_ren1->GetActiveCamera()->SetClippingRange(0.1,10000);
		m_view->CameraUpdate();
	}

  // Handle autoshow.
	// Now autoshow work also for mutex vme.
	// Autoshow for mutex vme work only if no other vme of the sme type are already shown.
	if( m_autoshow[type] && node->m_pipe_creatable && !m_shown_mutex_vme[type] )
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
  mafSceneNode *node = new mafSceneNode(this,parent,vme,m_ren1,m_ren2) ;

	// insert node in the list
	if(!m_list)
		m_list = node;
  else
  {
    for(mafSceneNode* n = m_list; n->m_next; n=n->m_next); // go to the end of the list
			n->m_next = node;
  }
  return node;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeRemove(mafNode *vme)   
//----------------------------------------------------------------------------
{
	VmeShow(vme,false);

  mafSceneNode *node;
	
	if(!m_list) return;

  if(m_list->m_vme == vme)
  {
     node = m_list;
		 m_list = m_list->m_next;
  }
  else
  {
    for(mafSceneNode *n = m_list; n->m_next; n=n->m_next) // find prec of node(vme)
    {
      if(n->m_next->m_vme == vme)
      {
        node = n->m_next;
        n->m_next = n->m_next->m_next;
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
  for(mafSceneNode *n = m_list; n; n=n->m_next)
    if(n->m_vme == vme)
      return n;
  return NULL;
}
//----------------------------------------------------------------------------
void mafSceneGraph::VmeSelect(mafNode *vme, bool select)   
//----------------------------------------------------------------------------
{
  if(select) m_selected_vme = vme;

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
  m_mutex[type] = flag;
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
  if(!node->m_pipe_creatable) return;
  if( node->IsVisible() == show) return;

	if(vme->IsA("mafNodeGizmo"))
  {
    //nothing
  }
  else
  {
		if(show)
		{	
      /* @@@		
      if(node->m_mutex)
			{
				// find other visible node with the same type and hide it
				//wxString type = vme->GetClassName();
        int type = mafGetBaseType(vme);
				for( mafSceneNode *n = m_list; n; n=n->m_next)
					//if(type == n->m_vme->GetClassName() && n->m_pipe != NULL )
          if(type == mafGetBaseType(n->m_vme) && n->m_pipe != NULL )
					  mafEventMacro(mafEvent(this,VME_SHOW,n->m_vme,false));
			}
      @@@ */
			if(!node->m_pipe) 
			{
				m_view->VmeCreatePipe(vme);
				VmeSelect(vme,vme == m_selected_vme);
			}
		}
		else
		{
			if(node->m_pipe) m_view->VmeDeletePipe(vme);
		}
  }
	//node->Show(show);  //SIL. 21-4-2005: 
  
  /* @@@
  // update m_shown_mutex_vme
	if(node->m_mutex)
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
	
	for( mafSceneNode *n = m_list; n; n=n->m_next)
	  if(type == mafGetBaseType(n->m_vme) && n->IsVisible() != show )
			//- mutex vme may be shown only is no other vme of the same type is currently shown.
			//- mutex vme may always be hidden.
			if(!show || m_shown_mutex_vme[type]==NULL) 
				mafEventMacro(mafEvent(this,VME_SHOW,n->m_vme,show));
}
@@@ */

/* @@@
//----------------------------------------------------------------------------
// Scan vme subtree and send a VME_SHOW event for each Creatable/non_mutex vme found.
// Changed behavoir.
// Mutex vme may be shown only is no other vme of the same type is currently shown.
// Mutex vme may always be hidden.
void mafSceneGraph::VmeShowSubTree(mafNode *vme,  bool show)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = vme->NewIterator();
	// attenzione a skippare i Gizmo

	for(mafNode *v=iter->GetFirstNode();v;v=iter->GetNextNode())
	{
    mafSceneNode *n = Vme2Node(v);
    mafNodeBaseTypes type = mafGetBaseType(v);

		if(n && n->m_pipe_creatable && n->IsVisible() != show )
		{
			// Mutex vme may be shown only is no other vme of the same type is currently shown.
			// Mutex vme may always br hidden.
			if( !show || m_shown_mutex_vme[type]==NULL)
					mafEventMacro(mafEvent(this, VME_SHOW, v, show));
		} 
	}
	iter->Delete();
}
@@@ */

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
	if(m_gui == NULL)
	{
		m_gui = new mmgGui(this);
		
		bool b1 = m_creatable[VME_SURFACE]	&& !m_mutex[VME_SURFACE];
		bool b2 = m_creatable[VME_VOLUME]		&& !m_mutex[VME_VOLUME];
		bool b3 = m_creatable[VME_GRAY_VOLUME]		&& !m_mutex[VME_GRAY_VOLUME];
		bool b4 = m_creatable[VME_IMAGE]		&& !m_mutex[VME_IMAGE];
		bool b5 = m_creatable[VME_POINTSET] && !m_mutex[VME_POINTSET];

		//modified by STEFY 30-4-2004(begin)
		bool b6 = m_creatable[VME_SCALAR] && !m_mutex[VME_SCALAR];
		//modified by STEFY 30-4-2004(end)
		
		//if(b1 || b2 || b3 || b4 || b5) m_gui->Label("display list",true);
		if(b1 || b2 || b3 || b4 || b5 || b6) m_gui->Label("display list",true);

		if(b1) m_gui->Bool(ID_AUTOSHOW_SURFACE,  "auto show all Surfaces",  &m_autoshow[VME_SURFACE], true);
		if(b2) m_gui->Bool(ID_AUTOSHOW_VOLUME,   "auto show all Volumes",   &m_autoshow[VME_VOLUME],  true);
		if(b3) m_gui->Bool(ID_AUTOSHOW_GRAY_VOLUME,   "auto show all Gray Volumes",   &m_autoshow[VME_GRAY_VOLUME],  true);
		if(b4) m_gui->Bool(ID_AUTOSHOW_IMAGE,    "auto show all Images",    &m_autoshow[VME_IMAGE],   true);
		if(b5) m_gui->Bool(ID_AUTOSHOW_POINTSET, "auto show all Landmarks", &m_autoshow[VME_POINTSET],true);
		
		//modified by STEFY 30-4-2004(begin)
		if(b5) m_gui->Bool(ID_AUTOSHOW_SCALAR, "auto show all Scalar", &m_autoshow[VME_SCALAR],true);
		//modified by STEFY 30-4-2004(end)

		m_gui->Update();
	}
	return m_gui;
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
	if(node->m_cloud->IsOpen()  && node->m_pipe)  m_view->VmeDeletePipe(node->m_cloud);
	if(!node->m_cloud->IsOpen() && !node->m_pipe && node->IsVisible())
	{
		m_view->VmeCreatePipe(node->m_cloud);
		VmeSelect(node->m_cloud,node->m_cloud== m_selected_vme);
  }
}
@@@ */

//----------------------------------------------------------------------------
int mafSceneGraph::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = Vme2Node(vme);
	if(!n)
    return NODE_NON_VISIBLE;

  /*
  bool creatable = n->m_pipe_creatable;
	//landmark are not creatable
	if(vme->IsA("mafNodeLandmark")) creatable = false;
  */
	//if(!creatable)											return NODE_NON_VISIBLE;
  //if( n->m_mutex &&  n->IsVisible())  return NODE_MUTEX_ON;
  //if( n->m_mutex && !n->IsVisible())  return NODE_MUTEX_OFF;
  if( n->IsVisible())									return NODE_VISIBLE_ON;
  if(!n->IsVisible())									return NODE_VISIBLE_OFF;
	return NODE_NON_VISIBLE;
}
