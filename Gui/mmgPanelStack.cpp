/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanelStack.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:02 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

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
  m_sizer->Remove(m_curr_panel);

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
  m_sizer->Remove(m_curr_panel);

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
