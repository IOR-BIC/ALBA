/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medGUIDynamicVP.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-10 15:29:13 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGUIDynamicVP.h"  
#include "mafGUI.h"
#include "mafGUIScrolledPanel.h"
#include "mafGUIValidator.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"
#include "mafView.h"

#include <wx/statline.h>

#include "mafDbg.h"

//----------------------------------------------------------------------------
// medGUIDynamicVP
//----------------------------------------------------------------------------
const wxEventType wxEVT_DEFERED_DELETE = wxNewEventType();

BEGIN_EVENT_TABLE(medGUIDynamicVP,mafGUIPanel)
  EVT_CUSTOM(wxEVT_DEFERED_DELETE, wxID_ANY, medGUIDynamicVP::OnDeferedDelete)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medGUIDynamicVP::medGUIDynamicVP( wxWindow* parent, wxWindowID id, long GUIstyle,
                             const wxPoint& pos, const wxSize& size, long style) : 
  mafGUIPanel(parent, id, pos, size, style), m_NotifyId(id)
//----------------------------------------------------------------------------
{
  m_SceneNode = NULL;
  m_Listener = NULL;
  m_VPipes = NULL;
  m_VPipe = NULL;    
  m_comboVP = NULL;
  m_VPipeIndex = -1; 
  
  //create items
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

  m_GUI_This = new wxPanel( this, wxID_ANY);
  m_GUI_This->SetExtraStyle(m_GUI_This->GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);
  m_GUI_This->SetSizer(CreateGUI(m_GuiStyle = GUIstyle));
  m_GUI_This->Layout();
  
  bSizerMain->Add( m_GUI_This, 0, wxEXPAND | wxALL, 0 );

  m_GUI_VP = new mafGUIScrolledPanel( this, wxID_ANY);  
  bSizerMain->Add( m_GUI_VP, 1, wxEXPAND | wxALL, 0 );

  this->SetSizer( bSizerMain );
  this->Layout();   

  m_GUI_This->TransferDataToWindow();
}

//----------------------------------------------------------------------------
medGUIDynamicVP::~medGUIDynamicVP()
//----------------------------------------------------------------------------
{  
  OnCloseVP();  
}


//------------------------------------------------------------------------
//Creates GUI (to be added into m_GUI_This) according to the given style. 
//This routine is called from SetStyle and from the ctor. 
//N.B. when called from ctor the overriden function is not 
//called but this one (C++ feature).
/*virtual*/ wxSizer* medGUIDynamicVP::CreateGUI(long style)
//------------------------------------------------------------------------
{
  m_comboVP = NULL;

  wxBoxSizer* bSizerGT = new wxBoxSizer( wxVERTICAL );

  if ((style & GS_NO_NAME) == 0)
  {
    wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );  
    bSizer1->Add( new wxStaticText( m_GUI_This, wxID_ANY, _("Name:"), 
      wxDefaultPosition, wxSize( 60,-1 )), 0, wxALL, 5 );

    wxTextCtrl* textName = new wxTextCtrl( m_GUI_This, ID_NAME, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 
      ((style & GS_READONLY_NAME) == 0 ? 0 : wxTE_READONLY) );

    textName->SetValidator(mafGUIValidator(this, ID_NAME, textName, &m_Name));

    bSizer1->Add( textName, 1, wxALL, 0 );
    bSizerGT->Add( bSizer1, 0, wxEXPAND, 0 );
  }

  if ((style & (GS_NO_CLOSE_VP | GS_NO_CREATE_VP)) != (GS_NO_CLOSE_VP | GS_NO_CREATE_VP))
  {
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    if ((style & GS_NO_CREATE_VP) == 0)
    {
      bSizer2->Add( new wxStaticText( m_GUI_This, wxID_ANY, 
        _("Visual Pipe:"), wxDefaultPosition, wxSize( 60,-1 )), 0, wxALL, 5 );

      m_comboVP = new wxComboBox( m_GUI_This, ID_CREATE_VP, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      m_comboVP->SetToolTip( _("Selects the visual pipe to be created.") );
      if (m_VPipes != NULL)
      {
        const SUPPORTED_VP_ENTRY* pEntry = m_VPipes;
        while (pEntry->szClassName != NULL) 
        {
          m_comboVP->Append(pEntry->szUserFriendlyName, (void*)pEntry->szClassName);
          pEntry++;
        }
      }

      m_comboVP->SetValidator(mafGUIValidator(this, ID_CREATE_VP, m_comboVP, &m_VPipeIndex));
      bSizer2->Add( m_comboVP, 1, wxALL, 1 );
    }

    if ((style & GS_NO_CLOSE_VP) == 0)
    {
      wxButton* bttnClose = new wxButton( m_GUI_This, ID_CLOSE_VP, 
        _("Close"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
      bttnClose->SetValidator(mafGUIValidator(this, ID_CLOSE_VP, bttnClose));
      bSizer2->Add( bttnClose, 0, wxALL, 0 );
    }

    bSizerGT->Add( bSizer2, 0, wxEXPAND, 0 );
  }

  if ((style & (GS_NO_CLOSE_VP | GS_NO_CREATE_VP | GS_NO_NAME)) != 
    (GS_NO_CLOSE_VP | GS_NO_CREATE_VP | GS_NO_NAME)) {  
    bSizerGT->Add( new wxStaticLine( m_GUI_This, wxID_ANY, 
      wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxALL, 5 );
  }

  return bSizerGT;
}


//------------------------------------------------------------------------
//Sets a new list of visual pipes. 
void medGUIDynamicVP::SetVPipesList(const SUPPORTED_VP_ENTRY* pList)
//------------------------------------------------------------------------
{
  if (m_VPipes != pList)
  {
    m_VPipes = pList;
    m_VPipeIndex = -1;       

    if (m_comboVP != NULL)
    {
      m_comboVP->Clear(); 
      if (m_VPipes != NULL)
      {
        const SUPPORTED_VP_ENTRY* pEntry = m_VPipes;
        while (pEntry->szClassName != NULL) 
        {
          m_comboVP->Append(pEntry->szUserFriendlyName, (void*)pEntry->szClassName);
          pEntry++;
        }
      }
    }
  
    //and create VP
    OnCreateVP();
  }
}

//------------------------------------------------------------------------
//Sets a new scene node for visual pipes. 
//N.B. currently constructed visual pipe is recreated, if needed.
void medGUIDynamicVP::SetSceneNode(mafSceneNode* node)
//------------------------------------------------------------------------
{
  if (m_SceneNode != node)
  {
    m_SceneNode = node;
    if (m_SceneNode != NULL)
      OnCreateVP(); //recreate the current visual pipe
  }
}

//------------------------------------------------------------------------
//Sets a new visual pipe
void medGUIDynamicVP::SetVPipeIndex(int nNewIndex)
//------------------------------------------------------------------------
{
  if (m_VPipeIndex != nNewIndex)
  {
    m_VPipeIndex = nNewIndex;
    if (m_SceneNode != NULL)
      OnCreateVP();
  }
}

//----------------------------------------------------------------------------
void medGUIDynamicVP::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {      
    case ID_NAME:
      NotifyListener(ID_NAME);
      break;

    case ID_CREATE_VP:
      OnCreateVP();    

      NotifyListener(ID_CREATE_VP);
      break;

    case ID_CLOSE_VP:
      OnCloseVP();

      NotifyListener(ID_CLOSE_VP);
      break;              

    default:
      e->Log();
      break;
    }         
  } 
}

//------------------------------------------------------------------------
//Notifies the listener, sending the specified notify id 
//and nData as and argument. 
/*virtual*/ void medGUIDynamicVP::NotifyListener(long nData)
{
  //notify the listener about change
  if (m_Listener != NULL)
  {
    mafEvent ev(this, m_NotifyId, nData);
    m_Listener->OnEvent(&ev);
  }
}

//------------------------------------------------------------------------
//Handles the construction of VP
/*virtual*/ void medGUIDynamicVP::OnCreateVP()
//------------------------------------------------------------------------
{
  wxCursor busy;
  
  if (m_VPipe != NULL) 
  {
    int nOldIndex = m_VPipeIndex;    
    
    OnCloseVP();

    m_VPipeIndex = nOldIndex;

    if (m_comboVP != NULL)
      m_comboVP->SetSelection(m_VPipeIndex);
  }
    
  _ASSERT(m_VPipe == NULL);
  if (m_VPipeIndex >= 0)
  {
    const char* szPipeName = m_VPipes[m_VPipeIndex].szClassName;
    CreateVisualPipe(szPipeName);

    if (m_VPipe == NULL)
      mafLogMessage(_("Cannot create visual pipe: '%s'"), szPipeName);
    else
    {
      mafGUI* pipeGUI = m_VPipe->GetGui();
      pipeGUI->FitGui();            

      pipeGUI->Reparent(m_GUI_VP);
      m_GUI_VP->Add(pipeGUI, 1, wxEXPAND);
      pipeGUI->Show(true);  //our gui is visible (if we are here)
      pipeGUI->Update();      
    }
  }
}

//------------------------------------------------------------------------
//Handles the closing of VP
/*virtual*/ void medGUIDynamicVP::OnCloseVP()
//------------------------------------------------------------------------
{
  //destroy GUI & pipe
  if (m_VPipe != NULL)
  {    
    wxWindow *current_gui = m_VPipe->GetGui();
    m_GUI_VP->Remove(current_gui);   
    current_gui->Show(false);
    current_gui->Reparent(mafGetFrame());

    DestroyVisualPipe();  //this will also destroy the GUI

    m_VPipeIndex = -1;
    if (m_comboVP != NULL)
      m_comboVP->SetSelection(m_VPipeIndex);

    m_GUI_VP->Layout();
    this->Layout();    
  }    
}

//------------------------------------------------------------------------
//Handles destroying of controls. Controls cannot be destroyed 
//immediately as they are referenced by wxWidgets event handing core.
//Its immediate destruction would cause crash.
void medGUIDynamicVP::OnDeferedDelete(wxEvent& event)
//------------------------------------------------------------------------
{
  //obj is sizer to be deleted
  wxSizer* sizer = dynamic_cast< wxSizer* >(event.GetEventObject());
  sizer->Clear();
  delete sizer;
}

//------------------------------------------------------------------------
//Sets a new style
void medGUIDynamicVP::SetGUIStyle(long newstyle)
//------------------------------------------------------------------------
{
  if (newstyle != m_GuiStyle)
  {
    wxSizer* oldSizer = m_GUI_This->GetSizer();
    m_GUI_This->SetSizer(CreateGUI(m_GuiStyle = newstyle), false);
    m_GUI_This->Layout();
    this->Layout();    

    if (oldSizer != NULL)
    {
      //oldSizer cannot be deleted here, it would lead to crash
      //if SetGUIStyle was called from some handing of control of the GUI
      //within oldSizer => we just hide it here and mark it to be deleted on idle
      oldSizer->Show(false);      
      
      wxCommandEvent myEvent(wxEVT_DEFERED_DELETE);
      myEvent.SetEventObject(oldSizer);

      this->AddPendingEvent(myEvent);   
    }

    m_GUI_This->TransferDataToWindow();
  }
}

//------------------------------------------------------------------------
//Sets a new name associated with the GUI.
void medGUIDynamicVP::SetName(const char* szNewName)
//------------------------------------------------------------------------
{  
  m_Name = szNewName;
  m_GUI_This->TransferDataToWindow();
}


//------------------------------------------------------------------------
//Constructs geometry visual pipe.
/*virtual*/ void medGUIDynamicVP::CreateVisualPipe(const char* classname)
//------------------------------------------------------------------------
{
  DestroyVisualPipe();

  mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();    
  mafPipe *pipe = (mafPipe*)pipe_factory->CreateInstance(classname);
  if (pipe != NULL)
  {
    pipe->SetListener(this->GetListener());
    pipe->Create(m_SceneNode);

    //if the view associated with this pipe contains only one VME
    //and one pipe, we will need to reset camera, so it is visible
    mafView* view = m_SceneNode->m_Sg->m_View;
    int nRefCount = medPipeRegister::RegisterPipe(pipe, view);
    if (nRefCount == 1 && view->GetNumberOfVisibleVME() == 1)
      view->CameraReset(m_SceneNode->m_Vme);    
    else
      view->CameraUpdate();
  }

  m_VPipe = pipe;
}

//------------------------------------------------------------------------
//Destroys geometry visual pipe.
/*virtual*/ void medGUIDynamicVP::DestroyVisualPipe()
//------------------------------------------------------------------------
{  
  medPipeRegister::UnregisterPipe(m_VPipe);
  cppDEL(m_VPipe); 
}

#pragma region Register of Pipes

//array of registered views-pipes
/*static*/ std::vector< medGUIDynamicVP::medPipeRegister::VIEW_ITEM > 
                          medGUIDynamicVP::medPipeRegister::m_RegViews;

//------------------------------------------------------------------------
//Registers the specified pipe with the view.
//Returns number of pipes registered for this view.
//N.B. duplicity check of pipes is not performed!
/*static*/ int medGUIDynamicVP::medPipeRegister::RegisterPipe(mafPipe* pipe, mafView* view)
//------------------------------------------------------------------------
{
  //try to find view
  int nIndex = FindView(view);
  if (nIndex < 0)
  {
    nIndex = (int)m_RegViews.size();

    VIEW_ITEM newitem;
    newitem.m_View = view;
    m_RegViews.push_back(newitem);    
  }

  VIEW_ITEM& item = m_RegViews[nIndex];
  item.m_Pipes.push_back(pipe);     //we do not check for the duplicities!
  return (int)item.m_Pipes.size();
}

//------------------------------------------------------------------------
//Unregisters the specified pipe with the view (if specified).
//Returns number of pipes registered for this view (after unregister).
/*static*/ int medGUIDynamicVP::medPipeRegister::UnregisterPipe(mafPipe* pipe, mafView* view)
//------------------------------------------------------------------------
{
  int nStartIndex, nEndIndex;
  if ((nEndIndex = FindView(view)) >= 0)
    nStartIndex = nEndIndex;
  else 
  {
    nStartIndex = 0; nEndIndex = (int)m_RegViews.size() - 1;
  }
  
  //search every view item to find the pipe
  for (int i = nStartIndex; i <= nEndIndex; i++)
  {
    VIEW_ITEM& item = m_RegViews[i];
    int nCount = (int)item.m_Pipes.size();
    for (int j = 0; j < nCount; j++)
    {
      if (item.m_Pipes[j] == pipe)
      {
        item.m_Pipes.erase(item.m_Pipes.begin() + j);
        return (int)item.m_Pipes.size();
      }
    }
  }

  return -1;  //error
}

//------------------------------------------------------------------------
//returns index of given view in m_RegViews, or -1, if not found
/*static*/ int medGUIDynamicVP::medPipeRegister::FindView(mafView* view)
//------------------------------------------------------------------------
{
  if (view != NULL)
  {
    int nCount = (int)m_RegViews.size();
    for (int i = 0; i < nCount; i++)
    {
      VIEW_ITEM& item = m_RegViews[i];
      if (item.m_View == view)
        return i;
    }
  }

  return -1;
}

#pragma endregion Register of Pipes