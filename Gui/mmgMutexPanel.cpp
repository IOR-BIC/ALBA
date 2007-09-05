/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgMutexPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.3 $
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

#include "mmgMutexPanel.h"
#include <wx/list.h>

//----------------------------------------------------------------------------
// mmgMutexPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgMutexPanel,mmgPanel)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgMutexPanel::mmgMutexPanel(wxWindow* parent,wxWindowID id)
:mmgPanel(parent,id)
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );
  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  //m_Sizer->Fit(this);
  //m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mmgMutexPanel::~mmgMutexPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmgMutexPanel::Put(wxWindow *win)
//----------------------------------------------------------------------------
{
  if(win == GetCurrentClient())
    return false; //SIL. 22-4-2005: -- if passing the current content, nothing happen

  RemoveCurrentClient();
  if(win == NULL)
  return false; //SIL. 22-4-2005: -- if passing NULL, the old content is removed 
  
  //win->FitGui(); // solo in mmgGUI
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
bool mmgMutexPanel::RemoveCurrentClient()
//----------------------------------------------------------------------------
{
  wxWindow *current_gui = GetCurrentClient();
  if(current_gui == NULL) 
    return false;  //nothing to remove

  m_Sizer->Detach(current_gui);
  current_gui->Show(false);
  current_gui->Reparent(mafGetFrame());
  this->Layout();
  return true;
}
//----------------------------------------------------------------------------
wxWindow *mmgMutexPanel::GetCurrentClient()
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
