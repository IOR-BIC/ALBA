/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIScrolledPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:39 $
  Version:   $Revision: 1.1 $
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


#include "mafGUIScrolledPanel.h"

//----------------------------------------------------------------------------
// mafGUIScrolledPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIScrolledPanel,wxScrolledWindow)
  //EVT_BUTTON (ID_EPANEL, wxEPanel::OnButton)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
mafGUIScrolledPanel::mafGUIScrolledPanel(wxWindow* parent,wxWindowID id)
:wxScrolledWindow(parent,id,wxDefaultPosition,wxDefaultSize,wxNO_BORDER) 
//----------------------------------------------------------------------------
{
  m_Sizer =  new wxBoxSizer( wxVERTICAL );

   //SIL. 31-3-2005: 
  this->SetScrollbars(0, 10, 0, 100); 
  this->EnableScrolling(0,1);         

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mafGUIScrolledPanel::~mafGUIScrolledPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafGUIScrolledPanel::Layout()
//----------------------------------------------------------------------------
{
  //SIL. 08-jun-2006 - not possible to call SetScrollbars here - stack overflow
  //this->SetScrollbars(0, 10,0, m_Sizer->GetMinSize().GetHeight()/10);
	return wxScrolledWindow::Layout();    
}
//----------------------------------------------------------------------------
void mafGUIScrolledPanel::Add(wxWindow* window,int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_Sizer->Add(window,option,flag,border);
  FitInside();
}
//----------------------------------------------------------------------------
void mafGUIScrolledPanel::Add(wxSizer*  sizer, int option, int flag, int border)  
//----------------------------------------------------------------------------
{
  m_Sizer->Add(sizer, option,flag,border);
  FitInside();
}
//----------------------------------------------------------------------------
bool mafGUIScrolledPanel::Remove(wxWindow* window)
//----------------------------------------------------------------------------
{
  return m_Sizer->Detach(window);
}
//----------------------------------------------------------------------------
bool mafGUIScrolledPanel::Remove(wxSizer*  sizer ) 
//----------------------------------------------------------------------------
{
  return m_Sizer->Detach(sizer);
}
//----------------------------------------------------------------------------
void mafGUIScrolledPanel::FitInside()
//----------------------------------------------------------------------------
{
  // Paolo 13-08-2007: fix resizing scrollbars to start from the
  // scrolled position and not from 0; this avoid jumps to the
  // scrollbar.
  int pos_old = GetScrollPos(wxVERTICAL);
  int range_old = GetScrollRange(wxVERTICAL);
  int range_new = m_Sizer->GetMinSize().GetHeight()/10;
  int pos_new = (pos_old * range_new) / range_old;
  this->SetScrollbars(0, 10,0, m_Sizer->GetMinSize().GetHeight()/10,0,pos_new);
  wxScrolledWindow::Layout();    
}
