/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUINamedPanel
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


#include "albaGUINamedPanel.h"
#include "albaDecl.h"
#include "albaPics.h"
#include "albaGUIPicButton.h"
//----------------------------------------------------------------------------
// albaGUINamedPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUINamedPanel,albaGUIPanel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

#define NamedPanelStyle wxNO_BORDER | wxCLIP_CHILDREN
//----------------------------------------------------------------------------
albaGUINamedPanel::albaGUINamedPanel( wxWindow* parent,wxWindowID id,bool CloseButton,bool HideTitle)
:albaGUIPanel(parent,id,wxDefaultPosition,wxDefaultSize,NamedPanelStyle)         
//----------------------------------------------------------------------------
{
  m_NextPanel = NULL;
  m_Sizer =  new wxBoxSizer( wxVERTICAL );

  m_Top = NULL;
  m_TopSizer = NULL;
  m_Label = NULL; 

  //m_Color = wxColour(133,162,185);
  m_Color = wxColour(110,150,200);
  //m_Color = wxColour(190,190,190);
  //m_Color = wxColour(255,255,255);

  if (!HideTitle)
  {
    //m_Top = new wxStaticBox( this, -1,"",wxDefaultPosition,wxSize(-1,30));
    m_Top = new wxPanel( this, -1);

    m_TopSizer =  new wxBoxSizer( wxHORIZONTAL );

	  m_Label = new albaGUILab( m_Top, ID_LABEL_CLICK, " Panel Title:");

	  wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
	  font.SetPointSize(10);
#endif
	  font.SetWeight(wxBOLD);
	  m_Label->SetFont(font);
	  m_TopSizer->Add(m_Label,1,wxEXPAND);

	  if (CloseButton)
	  {
		  albaGUIPicButton *b = new albaGUIPicButton(m_Top, "CLOSE_SASH",ID_CLOSE_SASH);
      b->SetEventId(ID_CLOSE_SASH);  //SIL. 7-4-2005: 
		  m_TopSizer->Add(b,0,wxRIGHT,2);
	  }

    m_Top->SetAutoLayout( true );
    m_Top->SetSizer( m_TopSizer );
	  m_TopSizer->Fit(m_Top);
	  m_TopSizer->SetSizeHints(m_Top);

	  m_Sizer->Add(m_Top,0,wxEXPAND|wxTOP,2);
  }
  
  SetTitleColor(&m_Color);

  this->SetAutoLayout( true );
  this->SetSizer( m_Sizer );
  m_Sizer->Fit(this);
  m_Sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
albaGUINamedPanel::~albaGUINamedPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUINamedPanel::SetTitleColor(wxColour *color)
//----------------------------------------------------------------------------
{
  if(color) m_Color = *color;
  if(m_Top) m_Top->SetBackgroundColour(m_Color);
  if(m_Label) m_Label->SetBackgroundColour(m_Color);
}
