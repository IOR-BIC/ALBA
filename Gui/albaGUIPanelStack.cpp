/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIPanelStack
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


#include "albaDecl.h"
#include "albaGUIPanelStack.h"
//----------------------------------------------------------------------------
// albaGUIPanelStack
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIPanelStack,albaGUIPanel)
    EVT_SIZE(albaGUIPanelStack::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaGUIPanelStack::albaGUIPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
               const wxSize& size, long style, const wxString& name)
:albaGUIPanel(parent,id,pos,size,style,name)         
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );

  m_CurrentPanel= new albaGUIPanel(this,-1); 
  m_NextPanel = NULL;

  Push(new albaGUIPanel(this,-1));

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
albaGUIPanelStack::~albaGUIPanelStack( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaGUIPanelStack::Put(albaGUIPanel* p)
//----------------------------------------------------------------------------
{
   Push(p);
   return true;
}
//----------------------------------------------------------------------------
bool albaGUIPanelStack::Remove(albaGUIPanel* p)
//----------------------------------------------------------------------------
{
  assert(p);
  assert(p==m_CurrentPanel);
  Pop();
  return true;
}
//----------------------------------------------------------------------------
void albaGUIPanelStack::Push(albaGUIPanel* p)
//----------------------------------------------------------------------------
{
  if(m_CurrentPanel == p ) return;

  assert(p);
  assert(m_CurrentPanel);
  m_CurrentPanel->Reparent(albaGetFrame());
  m_CurrentPanel->Show(false);
  m_Sizer->Detach(m_CurrentPanel);

  p->m_NextPanel = m_CurrentPanel;
  m_CurrentPanel= p;

  m_CurrentPanel->Show(true);
  m_CurrentPanel->Reparent(this);
  m_Sizer->Add(m_CurrentPanel,1,wxEXPAND);
   
  DoLayout();
}
//----------------------------------------------------------------------------
void albaGUIPanelStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(false);
  m_CurrentPanel->Reparent(albaGetFrame());
  m_Sizer->Detach(m_CurrentPanel);

  m_CurrentPanel= m_CurrentPanel->m_NextPanel;
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(true);
  m_CurrentPanel->Reparent(this);
  m_Sizer->Add(m_CurrentPanel,1,wxEXPAND);

  DoLayout();
}
//----------------------------------------------------------------------------
void albaGUIPanelStack::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void albaGUIPanelStack::DoLayout()
//----------------------------------------------------------------------------
{
  wxSizeEvent e;
	wxPanel::Layout();
}
