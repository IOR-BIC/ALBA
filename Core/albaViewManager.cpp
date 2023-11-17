/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewManager
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


#include "albaViewManager.h"
#include <wx/config.h>
#include "albaDecl.h"
#include "albaView.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMERoot.h"

#include "albaGUIMDIFrame.h"
#include "albaGUIMDIChild.h"
#include "albaGUIViewFrame.h"
#include "albaViewVTK.h"
#include "albaViewCompound.h"

#include "albaEvent.h"
#include "albaRWIBase.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaGUI.h"


/*
#include "typeinfo.h"
#include "vtkRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
*/
//----------------------------------------------------------------------------
albaViewManager::albaViewManager() :	albaGUISettings(this, _("View Manager"))
//----------------------------------------------------------------------------
{
  m_Mouse       = NULL;
  m_SelectedRWI = NULL;

	m_SettingsGui = NULL;
	InitializeSettings();

  m_ViewList          = NULL;
  m_Listener          = NULL;
  m_SelectedVme       = NULL;
  m_SelectedView      = NULL;
	m_RootVme           = NULL;
  m_ViewBeingCreated  = NULL; 
  m_TemplateNum       = 0;
  m_IdInvisibleMenuList.clear();

  for(int i=0; i<MAXVIEW; i++) 
    m_ViewTemplate[i] = NULL;

  for(int t=0; t<MAXVIEW; t++)
    for(int v=0; v<MAXVIEW; v++)
      m_ViewMatrixID[t][v] = NULL;
}
//----------------------------------------------------------------------------
albaViewManager::~albaViewManager()
{	
	if(m_SelectedRWI)
    m_SelectedRWI->SetMouse(NULL);
	
	albaView *v = NULL;

  while(m_ViewList) // destroy all created views
  {
    v = m_ViewList;
    m_ViewList = v->m_Next;
		((albaGUIMDIChild *)v->GetFrame())->Close();
  }

  for(int i=0; i<m_TemplateNum; i++) // destroy all template views
    cppDEL(m_ViewTemplate[i]);
}

//----------------------------------------------------------------------------
void albaViewManager::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
  m_Mouse = mouse;
  if(m_SelectedView && m_Mouse)
    m_Mouse->OnEvent(&albaEvent(this,VIEW_SELECT,m_SelectedView)); // Update the mouse for the selected view
}

//----------------------------------------------------------------------------
albaGUI* albaViewManager::GetSettingsGui()
{
	if (m_SettingsGui == NULL)
	{
		m_SettingsGui = new albaGUI(this);

		m_SettingsGui->Label("View Settings", true);
		m_SettingsGui->Label("");

 		m_SettingsGui->Bool(ID_VIEW_SETTING_MAXIMIZE, "Maximize on open", &m_ViewMaximize, 1);
		m_SettingsGui->Bool(ID_VIEW_SETTING_OPEN_ONLY_ONE, "Open only one view of each type", &m_ViewOpenOnlyOne, 1);
		m_SettingsGui->Divider(1);

		m_SettingsGui->Label("Background");
		m_SettingsGui->Color(ID_VIEW_SETTING_COLOR, "Color", &m_ViewColorBackground);
		m_SettingsGui->Button(ID_VIEW_SETTING_COLOR_DEFAULT, "Default Color");
		m_SettingsGui->Label("");
	}

	return m_SettingsGui;
}

//----------------------------------------------------------------------------
void albaViewManager::EnableViewSettings(bool enable /*= true*/)
{
	if (m_SettingsGui != NULL)
	{
		m_SettingsGui->Enable(ID_VIEW_SETTING_MAXIMIZE, enable);
		m_SettingsGui->Enable(ID_VIEW_SETTING_OPEN_ONLY_ONE, enable);
		m_SettingsGui->Enable(ID_VIEW_SETTING_COLOR, enable);
		m_SettingsGui->Enable(ID_VIEW_SETTING_COLOR_DEFAULT, enable);
	}
}

//----------------------------------------------------------------------------
void albaViewManager::InitializeSettings()
{
	m_ViewMaximize = 1;
	m_ViewOpenOnlyOne = 0;
		
	int m_Color[3] = { 71, 71, 71 };

	//On first run i cannot read configuration
	if (!m_Config->Read("View_Maximize", &m_ViewMaximize))
		m_Config->Write("View_Maximize", m_ViewMaximize); // So i will save default value

	if (!m_Config->Read("View_OpenOnlyOne", &m_ViewOpenOnlyOne))
		m_Config->Write("View_OpenOnlyOne", m_ViewOpenOnlyOne); // So i will save default value

	if (!m_Config->Read("View_ColorBackgroundR", &m_Color[0]))
		m_Config->Write("View_ColorBackgroundR", m_Color[0]); // So i will save default value

	if (!m_Config->Read("View_ColorBackgroundB", &m_Color[1]))
		m_Config->Write("View_ColorBackgroundB", m_Color[1]); // So i will save default value

	if (!m_Config->Read("View_ColorBackgroundG", &m_Color[2]))
		m_Config->Write("View_ColorBackgroundG", m_Color[2]); // So i will save default value

	m_ViewColorBackground = wxColor(m_Color[0], m_Color[1], m_Color[2]);

	m_Config->Flush();
}

//----------------------------------------------------------------------------
void albaViewManager::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
        albaView *view = e->GetView();
        //		albaRWIBase* rwi = (albaRWIBase*) e->GetWin();   
        //		if(!rwi) rwi = view->GetDefaultRWI();

        bool notifylogic = (view != m_SelectedView);

        ViewSelected(view/*, rwi*/);

        if(notifylogic)
          albaEventMacro(albaEvent(this,VIEW_SELECT)); // forward the view selection event to logic
      }
      break;

			case ID_VIEW_SETTING_MAXIMIZE:
			{
				m_Config->Write("View_Maximize", m_ViewMaximize);
				m_Config->Flush();
			}
			break;

			case ID_VIEW_SETTING_OPEN_ONLY_ONE:
			{
				m_Config->Write("View_OpenOnlyOne", m_ViewOpenOnlyOne);
				m_Config->Flush();
			}
			break;

			case ID_VIEW_SETTING_COLOR:
			{
				m_Config->Write("View_ColorBackgroundR", m_ViewColorBackground.Red());
				m_Config->Write("View_ColorBackgroundB", m_ViewColorBackground.Blue());
				m_Config->Write("View_ColorBackgroundG", m_ViewColorBackground.Green());
				m_Config->Flush();
			}
			break;
			case ID_VIEW_SETTING_COLOR_DEFAULT:
			{
				m_ViewColorBackground.Set(71, 71, 71);
				m_SettingsGui->Update();
				m_Config->Write("View_ColorBackgroundR", m_ViewColorBackground.Red());
				m_Config->Write("View_ColorBackgroundB", m_ViewColorBackground.Blue());
				m_Config->Write("View_ColorBackgroundG", m_ViewColorBackground.Green());
				m_Config->Flush();
			}
			break;
      default:
        albaEventMacro(*e); // forward up the event
      break; 
    }
	}
}
//----------------------------------------------------------------------------
void albaViewManager::ViewAdd(albaView *view, bool visibleInMenu)
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
void albaViewManager::ViewSelected(albaView *view/*, albaRWIBase *rwi*/)
{
  m_SelectedView = view;

//  if(m_SelectedRWI)
//    m_SelectedRWI->SetInteractorStyle(NULL);

  m_SelectedRWI = view->GetRWI();
  if (m_Mouse)
  {
    albaEvent e(this,VIEW_SELECT,m_SelectedView);
    e.SetChannel(MCH_OUTPUT);
    m_Mouse->OnEvent(&e); // update the mouse
  }
}
//----------------------------------------------------------------------------
void albaViewManager::VmeAdd(albaVME *vme)   
{
  for(albaView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeAdd(vme); // Add the VME in all the views

  wxString s;
  s = vme->GetTypeName();
  if(s == "albaVMERoot") // Add a root means add a new tree
  {
    m_RootVme     = (albaVMERoot*)vme;
    m_SelectedVme = vme; // Adding new tree, selected vme must be initialized at the root.
  }
}
//----------------------------------------------------------------------------
void albaViewManager::VmeRemove(albaVME *vme)   
{
  for(albaView* v = m_ViewList; v; v=v->m_Next) 
    v->VmeRemove(vme); // Remove the VME in all the views

	wxString s(vme->GetTypeName());
	if(s == "albaVMERoot") // Remove the root means remove the tree
  {
    m_RootVme     = NULL;
    m_SelectedVme = NULL;   // Removing the tree, selected vme must be set to NULL.
  }


	if (m_SelectedVme == vme)
		m_SelectedVme = NULL;
}
//----------------------------------------------------------------------------
void albaViewManager::VmeModified(albaVME *vme)
{
 //@@@ for(albaView* v = m_ViewList; v; v=v->m_next) 
 //@@@   v->VmeModified(vme);  -- view::vmeModified not exist now -- is this required ?
}
//----------------------------------------------------------------------------
void albaViewManager::VmeSelect(albaVME *vme)   
{
	if (vme != m_SelectedVme)
	{
		for (albaView* v = m_ViewList; v; v = v->m_Next)
		{
			if (m_SelectedVme)
				v->VmeSelect(m_SelectedVme, false); //deselect the previous selected vme

			v->VmeSelect(vme, true); // select the new one in the views
		}

		m_SelectedVme = vme; // set the new selected vme
	}

	CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewManager::VmeShow(albaVME *vme, bool show)   
{
 if(m_ViewBeingCreated) // Important - test m_ViewBeingCreated first
 {
   // show the vme in the view that has been created but
   // is not already inside the views' list (if exists)
   m_ViewBeingCreated->VmeShow(vme,show);
 }
 else
 {
	 if(m_SelectedView)
	 {
     m_SelectedView->VmeShow(vme,show); // show the vme in the selected view
	 }
 }
}
//----------------------------------------------------------------------------
void albaViewManager::PropertyUpdate(bool fromTag)
{
  for(albaView* v = m_ViewList; v; v=v->m_Next) 
		v->VmeUpdateProperty(this->m_SelectedVme, fromTag); // update the vme properties in all views
}
//----------------------------------------------------------------------------
void albaViewManager::CameraReset(bool sel)   
{
  if(m_SelectedView) m_SelectedView->CameraReset(sel ? m_SelectedVme : NULL); // reset the camera in the selected view
}
//----------------------------------------------------------------------------
void albaViewManager::CameraReset(albaVME *vme)   
{
  if(m_SelectedView) m_SelectedView->CameraReset(); // reset the camera in the selected view
}
//----------------------------------------------------------------------------
void albaViewManager::CameraUpdate(bool only_selected)   
{
  if (only_selected && m_SelectedView != NULL)
  {
    m_SelectedView->CameraUpdate(); // Update only the selected view
    return;
  }
  for(albaView* v = m_ViewList; v; v=v->m_Next) // Update all views
    v->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaViewManager::CameraFlyToMode()
{
//  if(m_is) m_is->FlyToMode();
}
//----------------------------------------------------------------------------
void albaViewManager::OnQuit()
{
}

//----------------------------------------------------------------------------
void albaViewManager::FillMenu(wxMenu* menu)
{
	for(int i=0; i<m_TemplateNum; i++)
	{
    albaView* v = m_ViewTemplate[i];
		if(IsVisibleInMenu(v))
		{
			albaGUI::AddMenuItem(menu,v->m_Id,v->GetLabel(),v->GetIcon());
		}
  }
}

//----------------------------------------------------------------------------
bool albaViewManager::IsVisibleInMenu(albaView* v)
{
  for(int i=0; i<m_IdInvisibleMenuList.size(); i++) // Search the specified view
  {
    if(m_IdInvisibleMenuList[i] == v->m_Id) return false;
  }
  
  return true;
}
//----------------------------------------------------------------------------
albaView *albaViewManager::ViewCreate(int id)
{
	albaView* new_view = NULL;
	albaView* view = NULL;
  int index = id - VIEW_START;

	if( index <0 || index > m_TemplateNum) // the specified template id is out of bound
	{
    assert(false);
		return NULL;
  }
	view = m_ViewTemplate[index];
  if(!view) return NULL;


	if (m_ViewOpenOnlyOne)
		for (albaView * view = GetList(); view; view = view->m_Next)
		{
			if (view->m_Id == id)
			{
				Activate(view);
				return view;
			}
		}
	
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

  albaEventMacro(albaEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame

	if (m_ViewMaximize)
		new_view->GetFrame()->Maximize();

	new_view->GetFrame()->SetMinSize(wxSize(500, 300));
	new_view->GetFrame()->Show(true); // show the view's frame
	new_view->SetBackgroundColor(m_ViewColorBackground);

	//Setting size to avoid wrong graphic draw 
	
	albaYield();

	return new_view;
}
//----------------------------------------------------------------------------
albaView *albaViewManager::ViewCreate(wxString label)
{
	albaView* new_view = NULL;
	albaView* view     = NULL;

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

	if(m_ViewOpenOnlyOne)
		for (albaView * view = GetList(); view; view = view->m_Next)
		{
			if (view->IsA(label)) return view;
		}

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

  albaEventMacro(albaEvent(this,VIEW_CREATED,new_view)); // ask Logic to create the frame
  
  new_view->GetFrame()->Show(true); // show the view's frame
	new_view->SetBackgroundColor(m_ViewColorBackground);

	return new_view;
}
//----------------------------------------------------------------------------
void albaViewManager::ViewInsert(albaView *view)
{
	view->SetListener(this);
  view->SetMouse(m_Mouse); // set the mouse for the specified view
  if(m_RootVme != NULL)
  {
    albaVMEIterator *iter = m_RootVme->NewIterator(); // iterate over inserted vme
    for(albaVME *vme = iter->GetFirstNode(); vme; vme = iter->GetNextNode())
			view->VmeAdd(vme); // Add them in the specified view
    iter->Delete();
  }

  if(m_SelectedVme)
    view->VmeSelect(m_SelectedVme,true); // select the vme in the inserted view

  if(!m_ViewList)
    m_ViewList = view;
  else
  {
    albaView* v;
    for(v = m_ViewList; v->m_Next; v = v->m_Next) ; // go on until the end of the list is reached.
    v->m_Next = view;
  }
}
//----------------------------------------------------------------------------
void albaViewManager::ViewDelete(albaView *view)
{
  if(m_SelectedView)
  {
    if(m_SelectedRWI)
      m_SelectedRWI->SetMouse(NULL); // update the selected view rwi
    m_SelectedRWI = NULL;
    m_SelectedView = NULL;
  }

  albaEventMacro(albaEvent(this,VIEW_DELETE,view)); // inform the sidebar
	
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
    for(albaView *v = m_ViewList; v; v = v->m_Next) // find previous(view)
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
void albaViewManager::ViewDeleteAll()
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
albaView *albaViewManager::GetSelectedView()
{
  return m_SelectedView; 
}
//----------------------------------------------------------------------------
void albaViewManager::Activate(albaView *view)
{
    ((albaGUIMDIChild *)view->GetFrame())->SetFocus();
}

//----------------------------------------------------------------------------
albaView * albaViewManager::GetFromList( const char *label )
{
  albaView *view;
  for(view = GetList(); view; view=view->m_Next)
    if (view->GetLabel()==label)
      break;
  return view;
}
