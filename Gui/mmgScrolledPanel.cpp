/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgScrolledPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:03 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgScrolledPanel.h"

//----------------------------------------------------------------------------
// mmgScrolledPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgScrolledPanel,wxScrolledWindow)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mmgScrolledPanel::mmgScrolledPanel(wxWindow* parent,wxWindowID id)
:wxScrolledWindow(parent,id,wxDefaultPosition,wxDefaultSize,wxNO_BORDER) 
//----------------------------------------------------------------------------
{
  m_sizer =  new wxBoxSizer( wxVERTICAL );

  this->SetScrollbars(0, 10, 0, 100);
  this->EnableScrolling(0,1);
  this->SetAutoLayout( TRUE );
  this->SetSizer( m_sizer );
  m_sizer->Fit(this);
  m_sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mmgScrolledPanel::~mmgScrolledPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmgScrolledPanel::Layout()
//----------------------------------------------------------------------------
{
  //this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10);
	return wxScrolledWindow::Layout();    
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::Add(wxWindow* window,int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_sizer->Add(window,option,flag,border);
  this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10);
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::Add(wxSizer*  sizer, int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_sizer->Add(sizer, option,flag,border);
  this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10);
}
//----------------------------------------------------------------------------
bool mmgScrolledPanel::Remove(wxWindow* window)
//----------------------------------------------------------------------------
{
  return m_sizer->Remove(window);
}
//----------------------------------------------------------------------------
bool mmgScrolledPanel::Remove(wxSizer*  sizer ) 
//----------------------------------------------------------------------------
{
  return m_sizer->Remove(sizer);
}
