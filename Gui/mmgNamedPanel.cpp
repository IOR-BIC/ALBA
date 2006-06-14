/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgNamedPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.7 $
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


#include "mmgNamedPanel.h"
#include "mafDecl.h"
#include "mafPics.h"
#include "mmgPicButton.h"
//----------------------------------------------------------------------------
// mmgNamedPanel
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgNamedPanel,mmgPanel)
END_EVENT_TABLE()
//----------------------------------------------------------------------------

#define NamedPanelStyle wxNO_BORDER | wxCLIP_CHILDREN
//----------------------------------------------------------------------------
mmgNamedPanel::mmgNamedPanel( wxWindow* parent,wxWindowID id,bool CloseButton,bool HideTitle)
:mmgPanel(parent,id,wxDefaultPosition,wxDefaultSize,NamedPanelStyle)         
//----------------------------------------------------------------------------
{
  m_next = NULL;
  m_sizer =  new wxBoxSizer( wxVERTICAL );

  m_top = NULL;
  m_topsizer = NULL;
  m_lab = NULL; 

  m_col = wxColour(133,162,185);
  m_col = wxColour(110,150,200);
  //m_col = wxColour(190,190,190);
  //m_col = wxColour(255,255,255);

  if (!HideTitle)
  {
    //m_top = new wxStaticBox( this, -1,"",wxDefaultPosition,wxSize(-1,30));
    m_top = new wxPanel( this, -1);

    m_topsizer =  new wxBoxSizer( wxHORIZONTAL );

	  m_lab = new mmgLab( m_top, ID_LABEL_CLICK, " Panel Title:");

	  wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
	  font.SetPointSize(9);
#endif
	  font.SetWeight(wxBOLD);
	  m_lab->SetFont(font);
	  m_topsizer->Add(m_lab,1,wxEXPAND);

	  if (CloseButton)
	  {
		  mmgPicButton *b = new mmgPicButton(m_top, "CLOSE_SASH",ID_CLOSE_SASH);
      b->SetEventId(ID_CLOSE_SASH);  //SIL. 7-4-2005: 
		  m_topsizer->Add(b,0,wxRIGHT,2);
	  }

    m_top->SetAutoLayout( TRUE );
    m_top->SetSizer( m_topsizer );
	  m_topsizer->Fit(m_top);
	  m_topsizer->SetSizeHints(m_top);

	  m_sizer->Add(m_top,0,wxEXPAND|wxTOP,2);
  }
  
  SetTitleColor(&m_col);

  this->SetAutoLayout( TRUE );
  this->SetSizer( m_sizer );
  m_sizer->Fit(this);
  m_sizer->SetSizeHints(this);
}
//----------------------------------------------------------------------------
mmgNamedPanel::~mmgNamedPanel( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgNamedPanel::SetTitleColor(wxColour *color)
//----------------------------------------------------------------------------
{
  if(color) m_col = *color;
  if(m_top) m_top->SetBackgroundColour(m_col);
  if(m_lab) m_lab->SetBackgroundColour(m_col);
}
