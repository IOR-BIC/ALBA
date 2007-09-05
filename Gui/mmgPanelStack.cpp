/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgPanelStack.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-05 08:26:02 $
  Version:   $Revision: 1.4 $
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
  m_Sizer =  new wxBoxSizer( wxVERTICAL );

  m_CurrentPanel= new mmgPanel(this,-1); 
  m_NextPanel = NULL;

  Push(new mmgPanel(this,-1));

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
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
  assert(p==m_CurrentPanel);
  Pop();
  return true;
}
//----------------------------------------------------------------------------
void mmgPanelStack::Push(mmgPanel* p)
//----------------------------------------------------------------------------
{
  if(m_CurrentPanel == p ) return;

  assert(p);
  assert(m_CurrentPanel);
  m_CurrentPanel->Reparent(mafGetFrame());
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
void mmgPanelStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(false);
  m_CurrentPanel->Reparent(mafGetFrame());
  m_Sizer->Detach(m_CurrentPanel);

  m_CurrentPanel= m_CurrentPanel->m_NextPanel;
  assert(m_CurrentPanel);

  m_CurrentPanel->Show(true);
  m_CurrentPanel->Reparent(this);
  m_Sizer->Add(m_CurrentPanel,1,wxEXPAND);

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
