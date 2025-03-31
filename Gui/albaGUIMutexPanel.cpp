/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIMutexPanel
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

#include "albaGUIMutexPanel.h"
#include <wx/list.h>

//----------------------------------------------------------------------------
// albaGUIMutexPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIMutexPanel,albaGUIPanel)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIMutexPanel::albaGUIMutexPanel(wxWindow* parent,wxWindowID id)
:albaGUIPanel(parent,id)
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );
  this->SetAutoLayout( true );
  this->SetSizer( m_Sizer );
  //m_Sizer->Fit(this);
  //m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
albaGUIMutexPanel::~albaGUIMutexPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaGUIMutexPanel::Put(wxWindow *win)
//----------------------------------------------------------------------------
{
  if(win == GetCurrentClient())
    return false; //SIL. 22-4-2005: -- if passing the current content, nothing happen

  RemoveCurrentClient();
  if(win == NULL)
  return false; //SIL. 22-4-2005: -- if passing NULL, the old content is removed 
  
  //win->FitGui(); // solo in albaGUI
  win->Reparent(this);
  m_Sizer->Add(win,1,wxEXPAND);
  win->Show(true);
  win->Update();
  
  //m_Sizer->Fit(this);
  //m_Sizer->SetSizeHints(this);
  this->Layout();

  return true;
}
//----------------------------------------------------------------------------
bool albaGUIMutexPanel::RemoveCurrentClient()
//----------------------------------------------------------------------------
{
  wxWindow *current_gui = GetCurrentClient();
  if(current_gui == NULL) 
    return false;  //nothing to remove

  m_Sizer->Detach(current_gui);
  current_gui->Show(false);
  current_gui->Reparent(albaGetFrame());
  this->Layout();
  return true;
}
//----------------------------------------------------------------------------
wxWindow *albaGUIMutexPanel::GetCurrentClient()
//----------------------------------------------------------------------------
{
  int num_children = this->GetChildren().GetCount();
  assert(num_children <= 1);

  if (num_children == 1)
  {
    wxWindowList::Node *node = this->GetChildren().GetFirst();
    return (wxWindow *)node->GetData();
  }
  return NULL;
}
