/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgGuiHolder.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgGuiHolder.h"
#include "mmgCheckListBox.h"
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
  m_panel = new mmgScrolledPanel(this,-1);
  Add(m_panel,1,wxEXPAND);
    
  m_currgui = NULL;
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
  if(m_currgui) Remove(m_currgui); 
  m_currgui = gui;
  m_currgui->FitGui();
  m_currgui->Reparent(m_panel);
  m_panel->Add(m_currgui,0,wxEXPAND);
  m_currgui->Show(true);
  m_currgui->Update();
  
  //m_panel->SetScrollbars(0, 10,0, m_currgui->GetMinSize().GetHeight()/10);//SIL. 28-9-2004: 
  m_panel->Layout(); 
  return true;
}
//----------------------------------------------------------------------------
bool mmgGuiHolder::Remove(mmgGui* gui)
//----------------------------------------------------------------------------
{
	if(m_currgui == NULL) return false; 
	if(gui==NULL ) gui = m_currgui;
	if(gui != m_currgui) return false; 

	m_currgui->Show(false);
	m_panel->Remove(m_currgui);
	m_currgui->Reparent(mafGetFrame());
	m_panel->Layout();    
	m_currgui = NULL;
	return true;
}
