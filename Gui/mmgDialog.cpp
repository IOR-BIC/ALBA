/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialog.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:01:22 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

//----------------------------------------------------------------------------
// Include: - include the class being defined first
//----------------------------------------------------------------------------
#include "mmgDialog.h"

#include "wx/busyinfo.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgDialog, wxDialog)
	EVT_CLOSE(mmgDialog::OnCloseWindow)
  EVT_SIZE (mmgDialog::OnSize)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mmgDialog::mmgDialog(const wxString& title,long style)
: wxDialog(mafGetFrame(), -1, title, wxDefaultPosition, wxDefaultSize, style)
//----------------------------------------------------------------------------
{
	m_enable_close_button = false;
}
//----------------------------------------------------------------------------
mmgDialog::~mmgDialog( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmgDialog::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
	if(m_enable_close_button)
	{
    wxDialog::OnCloseWindow(event);
	}
}
//----------------------------------------------------------------------------
void mmgDialog::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
	wxDialog::OnSize(event); 
	Refresh(false);
}
//----------------------------------------------------------------------------
void mmgDialog::EnableCloseButton(bool enable)
//----------------------------------------------------------------------------
{
  m_enable_close_button = enable;
}
