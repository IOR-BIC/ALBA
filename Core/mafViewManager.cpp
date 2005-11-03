/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-03 14:21:44 $
  Version:   $Revision: 1.20 $
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
#include "mafViewVTK.h"
#include "mafViewCompound.h"

#include "mafEvent.h"
#include "mafRWIBase.h"
#include "mmdMouse.h"

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
  m_SelectedVme       = NULL;
  m_SelectedView      = NULL;
	m_RootVme           = NULL;
  m_ViewBeingCreated  = NULL; 
  m_TemplateNum       = 0;
  for(int i=0; i<MAXVIEW; i++) 
    m_ViewTemplate[i] = NULL;

  // Paolo 2005-04-22
  for(int t=0; t<MAXVIEW; t++)
    for(int v=0; v<MAXVIEW; v++)
      m_ViewMatrixID[t][v] = NULL;
}
//----------------------------------------------------------------------------
mafViewManager::~mafViewManager( ) 
//----------------------------------------------------------------------------
{	
	if(m_SelectedRWI)
    m_SelectedRWI->SetMouse(NULL);
	
	mafView *v;

  while(m_ViewList)
  {
    v = m_ViewList;
    m_ViewList = v->m_Next;
    delete v;
  }

  for(int i=0; i<m_TemplateNum; i++) delete m_ViewTemplate[i];
}

//----------------------------------------------------------------------------
void mafViewManager::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
  if(m_SelectedView && m_Mouse)
    m_Mouse->OnEvent(&mafEvent(this,VIEW_SELECT,m_SelectedView));
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
        mafView*    view = e->GetView();
        //		mafRWIBase* rwi = (mafRWIBase*) e->GetWin();   
        //		if(!rwi) rwi = view->GetDefaultRWI();

        bool notifylogic = (view != m_SelectedView);

        ViewSelected(view/*, rwi*/);

        if(notifylogic )
          mafEventMacro(mafEvent(this,VIEW_SELECT,(long)e->GetSender())); 
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
	}
}
//----------------------------------------------------------------------------
void mafViewManager::ViewAdd(mafView *view)   
//----------------------------------------------------------------------------
{
  m_ViewTemplate[m_TemplateNum] = view;
	view->m_Id = m_TemplateNum + VIEW_START;
  view->m_Mult = 0; // Paolo 2005-04-22
  view->SetListener(this);

  m_TemplateNum++;    
}
//----------------------------------------------------------------------------
void mafViewManager::ViewSelected(mafView *view/*, mafRWIBase *rwi*/)
//----------------------------------------------------------------------------
{
  m_SelectedView = view;

  if(m_SelectedRWI)
    m_SelectedRWI->SetInteractorStyle(NULL);

  m_SelectedRWI = view->GetRWI();
  if (m_Mouse)
  {
    mafEvent e(this,VIEW_SELECT,m_SelectedView);
    e.SetChannel(MCH_OUTPUT);
    m_Mouse->OnEvent(&e);
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeAdd(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeAdd(n);

  wxString s;
  s = n->GetTypeName();
  if(s == "mafVMERoot") 
  {
    m_RootVme     = (mafVMERoot*)n;
    m_SelectedVme = n;     // Paolo 24-06-2004 Adding new tree, selected vme must be initialized at the root.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeRemove(mafNode *n)   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeRemove(n);

	wxString s(n->GetTypeName());
	if(s == "mafVMERoot") 
  {
    m_RootVme     = NULL;
    m_SelectedVme = NULL;   // Paolo 24-06-2004 Removing the tree, selected vme must be set to NULL.
  }
}
//----------------------------------------------------------------------------
void mafViewManager::VmeModified(mafNode *vme)
//---------------------------------------------------------------------------
{
 //@@@ for(mafView* v = m_ViewList; v; v=v->m_next) 
 //@@@   v->VmeModified(vme);  -- view::vmeModified not exist now -- is this required ?  //SIL. 21-4-2005: 
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
		m_SelectedVme = n;
	}
	for(mafView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeSelect(n,true); //select the new one
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewManager::VmeShow(mafNode *n, bool show)   
//----------------------------------------------------------------------------
{
 if(m_ViewBeingCreated) // Important - test m_ViewBeingCreated first
 {
   m_ViewBeingCreated->VmeShow(n,show);
 }
 else
 {
	 if(m_SelectedView)
	 {
     m_SelectedView->VmeShow(n,show);
	 }
 }
}
//----------------------------------------------------------------------------
void mafViewManager::PropertyUpdate(bool fromTag)
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
		v->VmeUpdateProperty(this->m_SelectedVme, fromTag);
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(bool sel)   
//----------------------------------------------------------------------------
{
  if(m_SelectedView) m_SelectedView->CameraReset(sel ? m_SelectedVme : NULL);
}
//----------------------------------------------------------------------------
void mafViewManager::CameraReset(mafNode *vme)   
//----------------------------------------------------------------------------
{
  //@@@ if(m_SelectedView) m_SelectedView->CameraReset(vme);
  if(m_SelectedView) m_SelectedView->CameraReset();
}
//----------------------------------------------------------------------------
void mafViewManager::CameraUpdate()   
//----------------------------------------------------------------------------
{
  for(mafView* v = m_ViewList; v; v=v->m_Next) 
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
void mafViewManager::FillMenu(wxMenu* menu)
//----------------------------------------------------------------------------
{
  wxString s;
  if (m_TemplateNum > 0)
  {
    menu->AppendSeparator();
  }
	for(int i=0; i<m_TemplateNum; i++)
	{
    mafView* v = m_ViewTemplate[i];
	  //s = wxString::Format("create new %s view",v->GetLabel().c_str());
    s = wxString::Format("%s",v->GetLabel().c_str());
	  menu->Append(v->m_Id, s, (wxMenu *)NULL, s );
  }
}
//----------------------------------------------------------------------------
mafView *mafViewManager::ViewCreate(int id)
//----------------------------------------------------------------------------
{
	mafView* new_view = NULL;
	mafView* view = NULL;
  int index = id - VIEW_START;

	if( index <0 || index > m_TemplateNum) 
	{
    assert(false); 
		return NULL;
  }
	view = m_ViewTemplate[index];
  if(!view) return NULL;

  // Paolo 2005-04-22
  int view_mult = 0;
  new_view = view->Copy(this);

  for(;view_mult < MAXVIEW; view_mult++)
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  view->m_Mult = view_mult;

  //update the matrix containing all created view
  m_ViewMatrixID[index][view_mult] = new_view;
  
	// during ViewInsert the View may send Events that will not be forwarded because 
	// the view isn't already selected - 
	m_ViewBeingCreated = new_view;
	ViewInsert(new_view);
	m_ViewBeingCreated = NULL;

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
  for(; index<m_TemplateNum; index++)
	{
    wxString t = typeid(*m_ViewTemplate[index]).name();
		if(t == type )
		{
	    view = m_ViewTemplate[index];
			break;
		}
	}
  assert(view);

  // Paolo 2005-04-22
  int view_mult = 0;
	new_view = view->Copy(this);

  for(;view_mult < MAXVIEW; view_mult++)
    if(m_ViewMatrixID[index][view_mult] == NULL)
      break;
  view->m_Mult = view_mult;

  //update the matrix containing all created view
  m_ViewMatrixID[index][view_mult] = new_view;

  m_ViewBeingCreated = new_view;
	ViewInsert(new_view);
	m_ViewBeingCreated = NULL;

  mafEventMacro(mafEvent(this,VIEW_CREATED,new_view));

	return new_view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewInsert(mafView *view)
//----------------------------------------------------------------------------
{
	view->SetListener(this);
  view->SetMouse(m_Mouse);
  if(m_RootVme != NULL)
  {
    mafNodeIterator *iter = m_RootVme->NewIterator();
    for(mafNode *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
			view->VmeAdd(vme);
    iter->Delete();
  }

  if(m_SelectedVme)
    view->VmeSelect(m_SelectedVme,true);

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
  mafEventMacro(mafEvent(this,VIEW_DELETE,view)); // inform the sidebar
	
  // Paolo 2005-04-22
  // calculate the view type index
  int index = view->m_Id - VIEW_START;
  // set to NULL the pointer into the state matrix
  m_ViewMatrixID[index][view->m_Mult] = NULL;

  if(!m_ViewList) return;
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

  if(m_SelectedView)
	{
    if(m_SelectedRWI) 
      //old m_selected_rwi->SetInteractorStyle(NULL); 
      m_SelectedRWI->SetMouse(NULL);
//old		m_is->SetInteractor(NULL);
//old    m_is->SetListener(this); // A. Savenko
    m_SelectedRWI = NULL;
    m_SelectedView = NULL;
	}
	delete view;
}
//----------------------------------------------------------------------------
void mafViewManager::ViewDeleteAll()
//----------------------------------------------------------------------------
{
	while(m_ViewList)
    m_ViewList->GetFrame()->Close();
}
//----------------------------------------------------------------------------
mafView *mafViewManager::GetSelectedView()
//----------------------------------------------------------------------------
{
  return m_SelectedView; 
}
