/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgGuiHolder.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-04 16:22:15 $
  Version:   $Revision: 1.14 $
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

#include "mmgGuiHolder.h"
#include <wx/list.h>
//@@@ #include "mmgCheckListBox.h"  //SIL. 29-3-2005: 

//----------------------------------------------------------------------------
// mmgGuiHolder
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgGuiHolder,mmgNamedPanel)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgGuiHolder::mmgGuiHolder(wxWindow* parent,wxWindowID id,bool CloseButton,bool HideTitle)
:mmgNamedPanel(parent,id,CloseButton,HideTitle) 
//----------------------------------------------------------------------------
{
  SetTitle(" module controls area:");
  m_Panel = new mmgScrolledPanel(this,-1);

  Add(m_Panel,1,wxEXPAND);
}
//----------------------------------------------------------------------------
mmgGuiHolder::~mmgGuiHolder( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmgGuiHolder::Put(mmgGui* gui)
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
bool mmgGuiHolder::RemoveCurrentGui()
//----------------------------------------------------------------------------
{
  wxWindow *current_gui = GetCurrentGui();
  if(current_gui == NULL) 
    return false;  //nothing to remove

  m_Panel->Remove(current_gui);
  current_gui->Show(false);
  current_gui->Reparent(mafGetFrame());
//  current_gui->Show(false);
  m_Panel->Layout();
  return true;
}
//----------------------------------------------------------------------------
wxWindow *mmgGuiHolder::GetCurrentGui()
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
