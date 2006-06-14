/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanelStack.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
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


#include "mafDecl.h"
#include "mmgPanelStack.h"
//----------------------------------------------------------------------------
// mmgPanelStack
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgPanelStack,mmgPanel)
    EVT_SIZE(mmgPanelStack::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmgPanelStack::mmgPanelStack(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
               const wxSize& size, long style, const wxString& name)
:mmgPanel(parent,id,pos,size,style,name)         
//----------------------------------------------------------------------------
{
  m_sizer =  new wxBoxSizer( wxVERTICAL );

  m_curr_panel= new mmgPanel(this,-1); 
  m_next = NULL;

  Push(new mmgPanel(this,-1));

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_sizer );
  m_sizer->Fit(this);
  m_sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mmgPanelStack::~mmgPanelStack( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmgPanelStack::Put(mmgPanel* p)
//----------------------------------------------------------------------------
{
   Push(p);
   return true;
}
//----------------------------------------------------------------------------
bool mmgPanelStack::Remove(mmgPanel* p)
//----------------------------------------------------------------------------
{
  assert(p);
  assert(p==m_curr_panel);
  Pop();
  return true;
}
//----------------------------------------------------------------------------
void mmgPanelStack::Push(mmgPanel* p)
//----------------------------------------------------------------------------
{
  if(m_curr_panel == p ) return;

  assert(p);
  assert(m_curr_panel);
  m_curr_panel->Reparent(mafGetFrame());
  m_curr_panel->Show(false);
  m_sizer->Detach(m_curr_panel);

  p->m_next = m_curr_panel;
  m_curr_panel= p;

  m_curr_panel->Show(true);
  m_curr_panel->Reparent(this);
  m_sizer->Add(m_curr_panel,1,wxEXPAND);
   
  DoLayout();
}
//----------------------------------------------------------------------------
void mmgPanelStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_curr_panel);

  m_curr_panel->Show(false);
  m_curr_panel->Reparent(mafGetFrame());
  m_sizer->Detach(m_curr_panel);

  m_curr_panel= m_curr_panel->m_next;
  assert(m_curr_panel);

  m_curr_panel->Show(true);
  m_curr_panel->Reparent(this);
  m_sizer->Add(m_curr_panel,1,wxEXPAND);

  DoLayout();
}
//----------------------------------------------------------------------------
void mmgPanelStack::OnSize(wxSizeEvent& WXUNUSED(event))
//----------------------------------------------------------------------------
{
  DoLayout();
}
//----------------------------------------------------------------------------
void mmgPanelStack::DoLayout()
//----------------------------------------------------------------------------
{
  wxSizeEvent e;
	wxPanel::OnSize(e);
}
