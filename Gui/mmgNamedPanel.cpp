/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgNamedPanel.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDecl.h"
#include "mmgNamedPanel.h"
#include "mmgBitmaps.h"
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

  if (!HideTitle)
  {
    m_top = new wxPanel( this, -1);
	  m_top->SetBackgroundColour(wxColour(133,162,185));
	  m_top->SetBackgroundColour(wxColour(110,150,200));
	  m_topsizer =  new wxBoxSizer( wxHORIZONTAL );

	  m_lab = new mmgLab( m_top, ID_LABEL_CLICK, " Panel Title:");
	  m_lab->SetBackgroundColour(wxColour(133,162,185));
	  m_lab->SetBackgroundColour(wxColour(110,150,200));

	  wxFont font = wxFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
	  font.SetPointSize(9);
	  font.SetWeight(wxBOLD);
	  m_lab->SetFont(font);
	  m_topsizer->Add(m_lab,1,wxEXPAND);

	  if (CloseButton)
	  {
		  mmgPicButton *b = new mmgPicButton(m_top, ID_CLOSE_SASH);
		  m_topsizer->Add(b,0,wxRIGHT,2);
	  }

    m_top->SetAutoLayout( TRUE );
    m_top->SetSizer( m_topsizer );
	  m_topsizer->Fit(m_top);
	  m_topsizer->SetSizeHints(m_top);

	  m_sizer->Add(m_top,0,wxEXPAND|wxTOP,2);
  }
   
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
