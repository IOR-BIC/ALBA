/*=========================================================================

 Program: MAF2
 Module: mafViewManager
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


#include "mafViewManager.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mafView.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVMERoot.h"

#include "mafGUIMDIFrame.h"
#include "mafGUIMDIChild.h"
#include "mafGUIViewFrame.h"
#include "mafViewVTK.h"
#include "mafViewCompound.h"

#include "mafEvent.h"
#include "mafRWIBase.h"
#include "mafDeviceButtonsPadMouse.h"

/*
#include "typeinfo.h"
#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
*/
//----------------------------------------------------------------------------
mafViewManager::mafViewManager()
//----------------------------------------------------------------------------
{
  m_Mouse       = NULL;
  m_SelectedRWI = NULL;

  m_ViewList          = NULL;
  m_Listener          = NULL;
  m_RemoteListener    = NULL;
  m_SelectedVme       = NULL;
  m_SelectedView      = NULL;
	m_RootVme           = NULL;
  m_ViewBeingCreated  = NULL; 
  m_TemplateNum       = 0;
  m_CollaborateStatus = false;
  m_FromRemote        = false;
  m_IdInvisibleMenuList.clear();
  for(int i=0; i<MAXVIEW; i++) 
    m_ViewTemplate[i] = NULL;

  for(int t=0; t<MAXVIEW; t++)
    for(int v=0; v<MAXVIEW; v++)
      m_ViewMatrixID[t][v] = NULL;
}
//----------------------------------------------------------------------------
mafViewManager::~mafViewManager()
//----------------------------------------------------------------------------
{	
	if(m_SelectedRWI)
    m_SelectedRWI->SetMouse(NULL);
	
	mafView *v = NULL;

  while(m_ViewList) // destroy all created views
  {
    v = m_ViewList;
    m_ViewList = v->m_Next;
    v->SetMouse(NULL); // dereference the mouse before destroy the view
    cppDEL(v);
  }

  for(int i=0; i<m_TemplateNum; i++) // destroy all template views
    cppDEL(m_ViewTemplate[i]);
}

//----------------------------------------------------------------------------
void mafViewManager::SetMouse(mafDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
  if(m_SelectedView && m_Mouse)
    m_Mouse->OnEvent(&mafEvent(this,VIEW_SELECT,m_SelectedView)); // Update the mouse for the selected view
}

//----------------------------------------------------------------------------
void mafViewManager::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case VIEW_CREATE:
        ViewCreate(e->GetId());
      break;
      case VIEW_DELETE:
        ViewDelete(e->GetView());
      break;
      case VIEW_SELECT:
      {
        mafView *view = e->GetView();
        //		mafRWIBase* rwi = (mafRWIBase*) e->GetWin();   
        //		if(!rwi) rwi = view->GetDefaultRWI();

        bool notifylogic = (view != m_SelectedView);

        ViewSelected(view/*, rwi*/);

        if(notifylogic)
          mafEventMacro(mafEvent(this,VIEW_SELECT,(long)e->GetSender())); // forward the view selection event to logic

        if(m_CollaborateStatus && m_RemoteListener && !m_FromRemote)
        {
          // Send the event to synchronize the remote application in case of collaboration modality
          mafEvent ev(this,VIEW_SELECTED,view);
          ev.SetChannel(REMOTE_COMMAND_CHANNEL);
          m_RemoteListener->OnEvent(&ev);
        }
      }
      break;
      default:
        mafEventMacro(*e); // forward up the event
      break; 
    }
	}
}
//----------------------------------------------------------------------------
void mafViewManager::ViewAdd(mafView *view, bool visibleInMenu)
//----------------------------------------------------------------------------
{
  m_ViewTemplate[m_TemplateNum] = view;
  // Update the view ID (starting from VIEW_START)
	view->m_Id = m_TemplateNum + VIEW_START;
  view->m_Mult = 0; // template views multiplicity is always 0
  view->SetListener(this);

  if (!visibleInMenu)
  {
    m_IdInvisibleMenuList.push_back(view->m_Id); // update the invisible to menu views vector
  }

  m_TemplateNum++;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewSelected(mafView *view/*, mafRWIBase *rwi*/)
//----------------------------------------------------------------------------
{
  m_SelectedView = view;

//  if(m_SelectedRWI)
//    m_SelectedRWI->SetInteractorStyle(NULL);

  m_SelectedRWI = view->GetRWI();
  if (m_Mouse)
  {
    mafEvent e(this,VIEW_SELECT,m_SelectedView);
    e.SetChannel(MCH_OUTPUT);
    m_Mouse->OnEvent(&e); // update the mouse
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeAdd(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeAdd(n); // Add the VME in all the views

  wxString s;
  s = n->GetTypeName();
  if(s == "mafVMERoot") // Add a root means add a new tree
  {
    m_RootVme     = (mafVMERoot*)n;
    m_SelectedVme = n; // Adding new tree, selected vme must be initialized at the root.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeRemove(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeRemove(n); // Remove the VME in all the views

	wxString s(n->GetTypeName());
	if(s == "mafVMERoot") // Remove the root means remove the tree
  {
    m_RootVme     = NULL;
    m_SelectedVme = NULL;   // Removing the tree, selected vme must be set to NULL.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeModified(mafNode *vme)
//---------------------------------------------------------------------------
{
 //@@@ for(mafView* v = m_ViewList; v; v=v->m_next) 
 //@@@   v->VmeModified(vme);  -- view::vmeModified not exist now -- is this required ?
}
//----------------------------------------------------------------------------
void mafViewManager::VmeSelect(mafNode *n)   
//----------------------------------------------------------------------------
{
	if(n != m_SelectedVme)
	{
		if(m_SelectedVme)
			for(mafView* v = m_ViewList; v; v=v->m_Next) 
				v->VmeSelect(m_SelectedVme,false); //deselect the previous selected vme
		m_SelectedVme = n; // set the new selected vme
	}
	for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeSelect(n,true); // select the new one in the views
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewManager::VmeShow(mafNode *n, bool show)   
//----------------------------------------------------------------------------
{
 if(m_ViewBeingCreated) // Important - test m_ViewBeingCreated first
 {
   // show the vme in the view that has been created but
   // is not already inside the views' list (if exists)
   m_ViewBeingCreated->VmeShow(n,show);
 }
 else
 {
	 if(m_SelectedView)
	 {
     m_SelectedView->VmeShow(n,show); // show the vme in the selected view
	 }
 }
}
//----------------------------------------------------------------------------
void mafViewManager::PropertyUpdate(bool fromTag)
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
		v->VmeUpdateProperty(this->m_SelectedVme, fromTag); // update the vme properties in all views
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(bool sel)   
//----------------------------------------------------------------------------
{
  if(m_SelectedView) m_SelectedView->CameraReset(sel ? m_SelectedVme : NULL); // reset the camera in the selected view
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(mafNode *vme)   
//----------------------------------------------------------------------------
{
  //@@@ if(m_SelectedView) m_SelectedView->CameraReset(vme);
  if(m_SelectedView) m_SelectedView->CameraReset(); // reset the camera in the selected view
}
//----------------------------------------------------------------------------
void mafViewManager::CameraUpdate(bool only_selected)   
//----------------------------------------------------------------------------
{
  if (only_selected && m_SelectedView != NULL)
  {
    m_SelectedView->CameraUpdate(); // Update only the selected view
    return;
  }
  for(mafView* v = m_ViewList; v; v=v->m_Next) // Update all views
    v->CameraUpdate();
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
void mafViewManager::FillMenu(wxMenu* menu)
//----------------------------------------------------------------------------
{
  wxString s;
	for(int i=0; i<m_TemplateNum; i++)
	{
    mafView* v = m_ViewTemplate[i];
    s = wxString::Format("%s",v->GetLabel().c_str());
    if(IsVisibleInMenu(v))
	    menu->Append(v->m_Id, s, (wxMenu *)NULL, s ); // Fill the menu with the labels of the template views
  }
}
//----------------------------------------------------------------------------
bool mafViewManager::IsVisibleInMenu(mafView* v)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_IdInvisibleMenuList.size(); i++) // Search the specified view
  {
    if(m_IdInvisibleMenuList[i] == v->m_Id) return false;
  }
  
  return true;
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(int id)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view = NULL;
  int index = id - VIEW_START;

	if( index <0 || index > m_TemplateNum) // the specified template id is out of bound
	{
    assert(false);
		return NULL;
  }
	view = m_ViewTemplate[index];
  if(!view) return NULL;

  // Paolo 2005-04-22
  int view_mult = 0;
  new_view = view->Copy(this); // the crated view is a copy of the specified template view

  for(;view_mult < MAXVIEW; view_mult++) // iterate over view matrix to calculate the multiplicity
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  new_view->m_Mult = view_mult;

  //update the matrix containing all created views
  m_ViewMatrixID[index][view_mult] = new_view;
  
	// during ViewInsert the View may send Events that will not be forwarded because 
	// the view isn't already selected - 
	m_ViewBeingCreated = new_view;
	ViewInsert(new_view);
	m_ViewBeingCreated = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame
	
	new_view->GetFrame()->Show(true); // show the view's frame

	return new_view;
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(wxString label)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view     = NULL;

	int index = 0;
  for(; index<m_TemplateNum; index++)
	{
    // find the template view of the specified type
    wxString t = m_ViewTemplate[index]->GetLabel();
		if(t == label )
		{
	    view = m_ViewTemplate[index];
			break;
		}
	}
  
  if (view==NULL) 
    return NULL;

  // Paolo 2005-04-22
  int view_mult = 0;
	new_view = view->Copy(this); // the crated view is a copy of the specified template view

  for(;view_mult < MAXVIEW; view_mult++)
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  view->m_Mult = view_mult; // update the view multiplicity

  //update the matrix containing all created view
  m_ViewMatrixID[index][view_mult] = new_view;

  m_ViewBeingCreated = new_view;
	ViewInsert(new_view);
	m_ViewBeingCreated = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame
  
  new_view->GetFrame()->Show(true); // show the view's frame

	return new_view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewInsert(mafView *view)
//----------------------------------------------------------------------------
{
	view->SetListener(this);
  view->SetMouse(m_Mouse); // set the mouse for the specified view
  if(m_RootVme != NULL)
  {
    mafNodeIterator *iter = m_RootVme->NewIterator(); // iterate over inserted vme
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
			view->VmeAdd(vme); // Add them in the specified view
    iter->Delete();
  }

  if(m_SelectedVme)
    view->VmeSelect(m_SelectedVme,true); // select the vme in the inserted view

  if(!m_ViewList)
    m_ViewList = view;
  else
  {
    mafView* v;
    for(v = m_ViewList; v->m_Next; v = v->m_Next) ; // go on until the end of the list is reached.
    v->m_Next = view;
  }
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDelete(mafView *view)
//----------------------------------------------------------------------------
{
  if(m_CollaborateStatus && m_RemoteListener && !m_FromRemote)
  {
    // Send the event to synchronize the remote application in case of collaboration modality
    mafEvent e(this,VIEW_DELETE,view);
    e.SetChannel(REMOTE_COMMAND_CHANNEL);
    m_RemoteListener->OnEvent(&e);
  }

  if(m_SelectedView)
  {
    if(m_SelectedRWI)
      m_SelectedRWI->SetMouse(NULL); // update the selected view rwi
    m_SelectedRWI = NULL;
    m_SelectedView = NULL;
  }

  mafEventMacro(mafEvent(this,VIEW_DELETE,view)); // inform the sidebar
	
  // Paolo 2005-04-22
  // calculate the view type index
  int index = view->m_Id - VIEW_START;
  // set to NULL the pointer into the state matrix
  m_ViewMatrixID[index][view->m_Mult] = NULL;

  if(!m_ViewList) return;

  // Remove the specified view from the views' list
  if(m_ViewList == view)
  {
    m_ViewList = view->m_Next;
  }
  else
  {
    for(mafView *v = m_ViewList; v; v = v->m_Next) // find previous(view)
    {
      if(v->m_Next == view)
      {
        v->m_Next = view->m_Next;
        break; 
      }
    }
  }

	cppDEL(view);
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDeleteAll()
//----------------------------------------------------------------------------
{
  if(m_SelectedView)
  {
    if(m_SelectedRWI) 
      m_SelectedRWI->SetMouse(NULL);
    m_SelectedRWI = NULL;
    m_SelectedView = NULL;
  }

  while(m_ViewList) // Close all views' frames
  {
    m_ViewList->GetFrame()->Show(false);
    m_ViewList->GetFrame()->Close();
  }
}
//----------------------------------------------------------------------------
mafView *mafViewManager::GetSelectedView()
//----------------------------------------------------------------------------
{
  return m_SelectedView; 
}
//----------------------------------------------------------------------------
void mafViewManager::Activate(mafView *view)
//----------------------------------------------------------------------------
{
  bool externalViewFlag;
  // Determine if is an external view
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Read("ExternalViewFlag", &externalViewFlag, false);
  cppDEL(config);

  // Set the focus to the frame of the specified view
  if(externalViewFlag)
    ((mafGUIViewFrame *)view->GetFrame())->SetFocus();
  else
    //((mafGUIMDIChild *)view->GetFrame())->Activate();
    ((mafGUIMDIChild *)view->GetFrame())->SetFocus();
}

//----------------------------------------------------------------------------
mafView * mafViewManager::GetFromList( const char *label )
//----------------------------------------------------------------------------
{
  mafView *view;
  for(view = GetList(); view; view=view->m_Next)
    if (view->GetLabel()==label)
      break;
  return view;
}
