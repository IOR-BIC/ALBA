/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgScrolledPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
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
  //SIL. 08-jun-2006 - not possible to call SetScrollbars here - stack overflow
  //this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10);
	return wxScrolledWindow::Layout();    
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::Add(wxWindow* window,int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_sizer->Add(window,option,flag,border);
  FitInside();
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::Add(wxSizer*  sizer, int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_sizer->Add(sizer, option,flag,border);
  FitInside();
}
//----------------------------------------------------------------------------
bool mmgScrolledPanel::Remove(wxWindow* window)
//----------------------------------------------------------------------------
{
  return m_sizer->Detach(window);
}
//----------------------------------------------------------------------------
bool mmgScrolledPanel::Remove(wxSizer*  sizer ) 
//----------------------------------------------------------------------------
{
  return m_sizer->Detach(sizer);
}
//----------------------------------------------------------------------------
void mmgScrolledPanel::FitInside()
//----------------------------------------------------------------------------
{
  this->SetScrollbars(0, 10,0, m_sizer->GetMinSize().GetHeight()/10);
  wxScrolledWindow::Layout();    
}
