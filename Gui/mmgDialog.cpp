/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialog.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:21 $
  Version:   $Revision: 1.2 $
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
