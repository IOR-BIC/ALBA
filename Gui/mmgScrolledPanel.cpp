/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgScrolledPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 08:58:09 $
  Version:   $Revision: 1.2 $
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

   //SIL. 31-3-2005: 
  this->SetScrollbars(0, 10, 0, 100); // vertical sb. only - this work
  this->EnableScrolling(0,1);         // vertical sb. only - this work
  //this->SetScrollbars(0, 100, 0, 100); 
  //this->EnableScrolling(1,1); 


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
  this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10); //this work
  //this->SetScrollbars(0, m_sizer->GetMinSize().GetWidth()/10,0, m_sizer->GetMinSize().GetHeight()/10); 
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::Add(wxSizer*  sizer, int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_sizer->Add(sizer, option,flag,border);
  this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10); //this work
  //this->SetScrollbars(0, m_sizer->GetMinSize().GetWidth()/10 ,0, m_sizer->GetMinSize().GetHeight()/10); //SIL. 31-3-2005: 
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
