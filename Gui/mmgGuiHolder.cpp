/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgGuiHolder.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-22 20:02:09 $
  Version:   $Revision: 1.5 $
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
  //m_panel = new wxScrolledWindow(this,-1);
  m_panel = new mmgScrolledPanel(this,-1);
  //m_panel->SetBackgroundColour(wxColour(251,251,253)); //SIL. 31-3-2005: 
  //m_panel->SetBackgroundColour(wxColour(255,0,0)); //SIL. 31-3-2005: 

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
  if(gui == m_currgui ) return false; //SIL. 22-4-2005: -- if passing the current content, nothing happen

  if(m_currgui) Remove(m_currgui); 
  m_currgui = gui;

  if( m_currgui == NULL ) return false; //SIL. 22-4-2005: -- if passing NULL, the old content is removed 

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
	if(m_currgui == NULL) return false;  //nothing to remove

  //SIL. 22-4-2005: hack -- vme_show(false) kills a pipe, the pipe kills its gui, and we have an invalid pointer
  //checking the parent allow to be more robust
  if(m_currgui->GetParent() != m_panel ) // m_currgui has been deleted, and is now invalid
  {
    m_currgui = NULL;
    return false;
  }

  if(gui==NULL ) gui = m_currgui; // if NULL was passed we remove the current gui

  if(gui != m_currgui) return false;  // if a non-NULL-Gui was specified  and is different from the current we do nothing

	m_panel->Remove(m_currgui);
	m_currgui->Reparent(mafGetFrame());
  m_currgui->Show(false);
	m_panel->Layout();    
	m_currgui = NULL;
	return true;
}
