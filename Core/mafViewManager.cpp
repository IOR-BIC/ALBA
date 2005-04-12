/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:06:47 $
  Version:   $Revision: 1.1 $
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
#include "mafNodeRoot.h"

//#include "mmgMDIFrame.h"
//#include "mmgMDIChild.h"

/*
#include "wx/busyinfo.h"
#include "mafInteractionDecl.h"
#include "mafEventBase.h"
#include "mafRWIBase.h"
#include "mafDevice.h"
#include "typeinfo.h"
*/
/*
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

  m_vlist        = NULL;
  m_Listener     = NULL;
  m_selected_vme = NULL;
  m_selected_view= NULL;
	m_root_vme     = NULL;
  m_view_being_created = NULL; 
  m_tcount       = 0;
  for(int i=0; i<MAXVIEW; i++) m_t[i]=NULL;
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
//@@@  for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@    v->VmeAdd(n);

  wxString s;
//  s = n->GetTypeName();
  if(s == "mafNodeRoot") 
  {
    m_root_vme     = (mafNodeRoot*)n;
    m_selected_vme = n;     // Paolo 24-06-2004 Adding new tree, selected vme must be initialized at the root.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeRemove(mafNode *n)   
//----------------------------------------------------------------------------
{
//@@@  for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@    v->VmeRemove(n);

	wxString s(n->GetTypeName());
	if(s == "mafNodeRoot") 
  {
    m_root_vme     = NULL;
    m_selected_vme = NULL;   // Paolo 24-06-2004 Removing the tree, selected vme must be set to NULL.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeModified(mafNode *vme)
//---------------------------------------------------------------------------
{
//@@@  for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@    v->VmeModified(vme);
}
//----------------------------------------------------------------------------
void mafViewManager::VmeSelect(mafNode *n)   
//----------------------------------------------------------------------------
{
	if(n != m_selected_vme)
	{
//@@@		if(m_selected_vme)
//@@@			for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@				v->VmeSelect(m_selected_vme,false);
		m_selected_vme = n;
	}
//@@@	for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@    v->VmeSelect(n,true);
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewManager::VmeShow(mafNode *n, bool show)   
//----------------------------------------------------------------------------
{
 if(m_view_being_created) // Important - test m_view_being_created first
 {
//@@@   m_view_being_created->VmeShow(n,show);
 }
 else
 {
	 if(m_selected_view)
	 {
//@@@     m_selected_view->VmeShow(n,show);
	 }
 }
}
//----------------------------------------------------------------------------
void mafViewManager::PropertyUpdate(bool fromTag)
//----------------------------------------------------------------------------
{
//@@@			for(mafView* v = m_vlist; v; v=v->m_next) 
//@@@		v->VmeUpdateProperty(this->m_selected_vme, fromTag);
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(bool sel)   
//----------------------------------------------------------------------------
{
//@@@  if(m_selected_view) m_selected_view->CameraReset( sel ? m_selected_vme : NULL);
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(mafNode *vme)   
//----------------------------------------------------------------------------
{
//@@@  if(m_selected_view) m_selected_view->CameraReset(vme);
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
	  //s = wxString::Format("create new %s view",v->m_label);
	  menu->Append(v->m_id, v->m_label, (wxMenu *)NULL, v->m_label);
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

  //@@@//@@@//@@@//@@@//@@@//@@@ new_view = view->Copy(this);
  
/*
  for (int idx = 0; idx < view->m_CustomPipeVmeList.size(); idx ++)
    new_view->m_CustomPipeVmeList.push_back(view->m_CustomPipeVmeList[idx]);
*/

	// during ViewInsert the View may send Events that will not be forwarded because 
	// the view isn't already selected - 
	m_view_being_created = new_view;

	ViewInsert(new_view);

	m_view_being_created = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame
	
	new_view->m_frame->Show(true);

	return new_view;
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(wxString type)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view = NULL;

	for(int i=0; i<m_tcount; i++)
	{
    wxString t = typeid(*m_t[i]).name();
		if(t == type )
		{
	    view = m_t[i];
			break;
		}
	}

	assert(view);
//@@@//@@@//@@@//@@@//@@@//@@@	new_view = view->Copy(this);
/*
  for (int idx = 0; idx < view->m_CustomPipeVmeList.size(); idx ++)
    new_view->m_CustomPipeVmeList.push_back(view->m_CustomPipeVmeList[idx]);
*/
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

/*//@@@
  if(m_root_vme != NULL)
  {
    mafNodeIterator *iter = m_root_vme->NewIterator();
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
			view->VmeAdd(vme);
    iter->Delete();
  }
*/
//@@@  if(m_selected_vme)
//@@@    view->VmeSelect(m_selected_vme,true);

  if(!m_vlist)
    m_vlist = view;
  else
  {
    for(mafView* v = m_vlist; v->m_next; v = v->m_next) ; // go on until the end of the list is reached.
    v->m_next = view;
  }
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDelete(mafView *view)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VIEW_DELETE,view));
	
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
		m_selected_view = NULL;
    m_selected_rwi = NULL;
    */
	}
	delete view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDeleteAll()
//----------------------------------------------------------------------------
{
	while(m_vlist)
    m_vlist->m_frame->Close();
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
