/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-23 09:53:15 $
  Version:   $Revision: 1.8 $
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


#include "mafViewManager.h"
#include "mafDecl.h"
#include "mafView.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVMERoot.h"

#include "mmgMDIFrame.h"
#include "mmgMDIChild.h"

/*
#include "wx/busyinfo.h"
#include "mafInteractionDecl.h"
#include "mafEventBase.h"
#include "mafRWIBase.h"
#include "mafDevice.h"
#include "typeinfo.h"
#include "mafAction.h"
#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
*/
//----------------------------------------------------------------------------
mafViewManager::mafViewManager()
//----------------------------------------------------------------------------
{
  //m_MouseAction  = NULL;
  //m_selected_rwi = NULL;
  //m_is=NULL;

  m_vlist              = NULL;
  m_Listener           = NULL;
  m_selected_vme       = NULL;
  m_selected_view      = NULL;
	m_root_vme           = NULL;
  m_view_being_created = NULL; 
  m_tcount       = 0;
  for(int i=0; i<MAXVIEW; i++) 
    m_t[i]=NULL;

  // Paolo 2005-04-22
  for(int t=0; t<MAXVIEW; t++)
    for(int v=0; v<MAXVIEW; v++)
      m_ViewMatrixID[t][v] = NULL;
}
//----------------------------------------------------------------------------
mafViewManager::~mafViewManager( ) 
//----------------------------------------------------------------------------
{
	/*
	if(m_selected_rwi)
    m_selected_rwi->SetMouseAction(NULL);
	*/
	mafView *v;

  while(m_vlist)
  {
    v = m_vlist;
    m_vlist = v->m_next;
    delete v;
  }

  for(int i=0; i<m_tcount; i++) delete m_t[i];
}
/*
//----------------------------------------------------------------------------
void mafViewManager::SetMouseAction(mafAction *action)
//----------------------------------------------------------------------------
{
  m_MouseAction = action;
  if(m_selected_view)
    m_MouseAction->ProcessEvent(mafSmartEvent(this,ViewSelectedEvent,m_selected_view));
}
*/
//----------------------------------------------------------------------------
void mafViewManager::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
	switch(e.GetId())
	{
    case VIEW_CREATE:
			ViewCreate(e.GetId());
		break;
    case VIEW_DELETE:
			ViewDelete(e.GetView());
		break;
		case VIEW_SELECT:
		{
			mafView*    view = e.GetView();
	//		mafRWIBase* rwi = (mafRWIBase*) e.GetWin();   
	//		if(!rwi) rwi = view->GetDefaultRWI();
			
      bool notifylogic = (view != m_selected_view);

			ViewSelected(view/*, rwi*/);

			if(notifylogic )
				mafEventMacro(mafEvent(this,VIEW_SELECT,(long)e.GetSender())); 
    }
    break;
    default:
      mafEventMacro(e);
    break; 
	}
}
//----------------------------------------------------------------------------
void mafViewManager::ViewAdd(mafView *view)   
//----------------------------------------------------------------------------
{
  m_t[m_tcount] = view;
	view->m_id = m_tcount + VIEW_START;
  view->m_mult = 0; // Paolo 2005-04-22
  view->SetListener(this);

  m_tcount++;    
}
//----------------------------------------------------------------------------
void mafViewManager::ViewSelected(mafView *view/*, mafRWIBase *rwi*/)
//----------------------------------------------------------------------------
{
  m_selected_view = view;

  //if(m_selected_rwi)
  //  m_selected_rwi->SetInteractorStyle(NULL);
  //m_selected_rwi = rwi;

  //m_MouseAction->ProcessEvent(mafSmartEvent(this,ViewSelectedEvent,m_selected_view),mafDevice::DeviceOutputChannel);
}
//----------------------------------------------------------------------------
void mafViewManager::VmeAdd(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_vlist; v; v=v->m_next) 
    v->VmeAdd(n);

  wxString s;
  s = n->GetTypeName();
  if(s == "mafVMERoot") 
  {
    m_root_vme     = (mafVMERoot*)n;
    m_selected_vme = n;     // Paolo 24-06-2004 Adding new tree, selected vme must be initialized at the root.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeRemove(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_vlist; v; v=v->m_next) 
    v->VmeRemove(n);

	wxString s(n->GetTypeName());
	if(s == "mafVMERoot") 
  {
    m_root_vme     = NULL;
    m_selected_vme = NULL;   // Paolo 24-06-2004 Removing the tree, selected vme must be set to NULL.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeModified(mafNode *vme)
//---------------------------------------------------------------------------
{
 //@@@ for(mafView* v = m_vlist; v; v=v->m_next) 
 //@@@   v->VmeModified(vme);  -- view::vmeModified not exist now -- is this required ?  //SIL. 21-4-2005: 
}
//----------------------------------------------------------------------------
void mafViewManager::VmeSelect(mafNode *n)   
//----------------------------------------------------------------------------
{
	if(n != m_selected_vme)
	{
		if(m_selected_vme)
			for(mafView* v = m_vlist; v; v=v->m_next) 
				v->VmeSelect(m_selected_vme,false); //deselect the previous selected vme
		m_selected_vme = n;
	}
	for(mafView* v = m_vlist; v; v=v->m_next) 
    v->VmeSelect(n,true); //select the new one
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewManager::VmeShow(mafNode *n, bool show)   
//----------------------------------------------------------------------------
{
 if(m_view_being_created) // Important - test m_view_being_created first
 {
   m_view_being_created->VmeShow(n,show);
 }
 else
 {
	 if(m_selected_view)
	 {
     m_selected_view->VmeShow(n,show);
	 }
 }
}
//----------------------------------------------------------------------------
void mafViewManager::PropertyUpdate(bool fromTag)
//----------------------------------------------------------------------------
{
  for(mafView* v = m_vlist; v; v=v->m_next) 
		v->VmeUpdateProperty(this->m_selected_vme, fromTag);
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(bool sel)   
//----------------------------------------------------------------------------
{
  //@@@ if(m_selected_view) m_selected_view->CameraReset( sel ? m_selected_vme : NULL);
  if(m_selected_view) m_selected_view->CameraReset();
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(mafNode *vme)   
//----------------------------------------------------------------------------
{
  //@@@ if(m_selected_view) m_selected_view->CameraReset(vme);
  if(m_selected_view) m_selected_view->CameraReset();
}
//----------------------------------------------------------------------------
void mafViewManager::CameraUpdate()   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_vlist; v; v=v->m_next) 
    v->CameraUpdate();;
}
//----------------------------------------------------------------------------
void mafViewManager::CameraFlyToMode()   
//----------------------------------------------------------------------------
{
//  if(m_is) m_is->FlyToMode();
}
//----------------------------------------------------------------------------
void mafViewManager::OnQuit()   
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafViewManager::FillMenu (wxMenu* menu)
//----------------------------------------------------------------------------
{
  wxString s;
	for(int i=0; i<m_tcount; i++)
	{
    mafView* v = m_t[i];  
	  s = wxString::Format("create new %s view",v->GetLabel());
	  menu->Append(v->m_id, s, (wxMenu *)NULL, s );	
  }
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(int id)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view = NULL;
  int index = id - VIEW_START;

	if( index <0 || index > m_tcount) 
	{
    assert(false); 
		return NULL;
  }
	view = m_t[index];
  if(!view) return NULL;

  // Paolo 2005-04-22
  int view_id   = view->m_id;
  int view_mult = 0;
  new_view = view->Copy(this);
  new_view->m_id = view_id;

  for(;view_mult < MAXVIEW; view_mult++)
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  view->m_mult = view_mult;

  //update the matrix containing all created view
  m_ViewMatrixID[index][view_mult] = new_view;
  
  new_view->m_PipeMap = view->m_PipeMap;

	// during ViewInsert the View may send Events that will not be forwarded because 
	// the view isn't already selected - 
	m_view_being_created = new_view;
	ViewInsert(new_view);
	m_view_being_created = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame
	
	new_view->GetFrame()->Show(true);

	return new_view;
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(wxString type)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view     = NULL;

	int index = 0;
  for(; index<m_tcount; index++)
	{
    wxString t = typeid(*m_t[index]).name();
		if(t == type )
		{
	    view = m_t[index];
			break;
		}
	}
  assert(view);

  // Paolo 2005-04-22
  int view_id   = view->m_id;
  int view_mult = 0;
	new_view = view->Copy(this);
  new_view->m_id = view_id;

  for(;view_mult < MAXVIEW; view_mult++)
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  view->m_mult = view_mult;

  //update the matrix containing all created view
  m_ViewMatrixID[index][view_mult] = new_view;

  new_view->m_PipeMap = view->m_PipeMap;

	m_view_being_created = new_view;
	ViewInsert(new_view);
	m_view_being_created = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view));

	return new_view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewInsert(mafView *view)
//----------------------------------------------------------------------------
{
	view->SetListener(this);
//@@@  view->SetMouseAction(m_MouseAction);

  if(m_root_vme != NULL)
  {
    mafNodeIterator *iter = m_root_vme->NewIterator();
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
			view->VmeAdd(vme);
    iter->Delete();
  }

  if(m_selected_vme)
    view->VmeSelect(m_selected_vme,true);

  if(!m_vlist)
    m_vlist = view;
  else
  {
    mafView* v;
    for(v = m_vlist; v->m_next; v = v->m_next) ; // go on until the end of the list is reached.
    v->m_next = view;
  }
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDelete(mafView *view)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VIEW_DELETE,view)); // inform the sidebar
	
  // Paolo 2005-04-22
  // calculate the view type index
  int index = view->m_id - VIEW_START;
  // set to NULL the pointer into the state matrix
  m_ViewMatrixID[index][view->m_mult] = NULL;

  if(!m_vlist) return;
  if(m_vlist == view)
  {
    m_vlist = view->m_next;
  }
  else
  {
    for(mafView* v = m_vlist; v; v = v->m_next) // find previous(view)
    {
      if(v->m_next == view)
      {
        v->m_next = view->m_next;
        break; 
      }
    }
  }

  if(m_selected_view)
	{
/*@@@
    if(m_selected_rwi) 
      //old m_selected_rwi->SetInteractorStyle(NULL); 
      m_selected_rwi->SetMouseAction(NULL);
//old		m_is->SetInteractor(NULL);
//old    m_is->SetListener(this); // A. Savenko
    m_selected_rwi = NULL;
    */
    m_selected_view = NULL;
	}
	delete view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDeleteAll()
//----------------------------------------------------------------------------
{
	while(m_vlist)
    m_vlist->GetFrame()->Close();
}
//----------------------------------------------------------------------------
mafView *mafViewManager::GetSelectedView()
//----------------------------------------------------------------------------
{
  return m_selected_view; 
}
/*
//----------------------------------------------------------------------------
mafRWIBase *mafViewManager::GetSelectedRWI()
//----------------------------------------------------------------------------
{
  return m_selected_rwi; 
}
*/
//----------------------------------------------------------------------------
void mafViewManager::EnableSelect(bool enable)
//---------------------------------------------------------------------------
{
//  m_is->EnableSelect(enable);
}