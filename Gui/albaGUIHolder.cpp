/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIHolder
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

#include "albaGUIHolder.h"
#include <wx/list.h>
//@@@ #include "albaGUICheckListBox.h"  //SIL. 29-3-2005: 

//----------------------------------------------------------------------------
// albaGUIHolder
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIHolder,albaGUINamedPanel)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIHolder::albaGUIHolder(wxWindow* parent,wxWindowID id,bool CloseButton,bool HideTitle)
:albaGUINamedPanel(parent,id,CloseButton,HideTitle) 
//----------------------------------------------------------------------------
{
  SetTitle(" module controls area:");
  m_Panel = new albaGUIScrolledPanel(this,-1);

  Add(m_Panel,1,wxEXPAND);
}
//----------------------------------------------------------------------------
albaGUIHolder::~albaGUIHolder( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaGUIHolder::Put(albaGUI* gui)
//----------------------------------------------------------------------------
{
  if(gui == GetCurrentGui())
    return false; //SIL. 22-4-2005: -- if passing the current content, nothing happen

  RemoveCurrentGui();
  if(gui == NULL)
    return false; //SIL. 22-4-2005: -- if passing NULL, the old content is removed 
  gui->FitGui();
  gui->Reparent(m_Panel);
  m_Panel->Add(gui,1,wxEXPAND);
  gui->Show(true);
  gui->Update();
  
  m_Panel->Layout();
  gui->SetFocus();
  return true;
}
//----------------------------------------------------------------------------
bool albaGUIHolder::RemoveCurrentGui()
//----------------------------------------------------------------------------
{
  wxWindow *current_gui = GetCurrentGui();
  if(current_gui == NULL) 
    return false;  //nothing to remove

  m_Panel->Remove(current_gui);
  current_gui->Show(false);
  current_gui->Reparent(albaGetFrame());
//  current_gui->Show(false);
  m_Panel->Layout();
  return true;
}
//----------------------------------------------------------------------------
wxWindow *albaGUIHolder::GetCurrentGui()
//----------------------------------------------------------------------------
{
  int num_children = m_Panel->GetChildren().GetCount();
  assert(num_children <= 1);

  if (num_children == 1)
  {
    wxWindowList::Node *node = m_Panel->GetChildren().GetFirst();
    return (wxWindow *)node->GetData();
  }
  return NULL;
}
