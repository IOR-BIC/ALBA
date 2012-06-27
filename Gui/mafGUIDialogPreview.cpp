/*=========================================================================

 Program: MAF2
 Module: mafGUIDialogPreview
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUIDialogPreview.h"
#include "mafDecl.h"
#include "mafRWI.h"

//----------------------------------------------------------------------------
mafGUIDialogPreview::mafGUIDialogPreview(const wxString& title,long style)
: mafGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_PreviewSizer = new wxBoxSizer( wxHORIZONTAL );
  m_RwiSizer = new wxBoxSizer( wxVERTICAL );
  m_Rwi = NULL;
  m_Gui = NULL;

  if( style & mafUSERWI )
  {
    m_Rwi = new mafRWI(this);
    m_Rwi->SetSize(0,0,500,500);
    m_Rwi->Show(true);
    m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_PreviewSizer->Add(m_RwiSizer,1,wxEXPAND);
  }
  if( style & mafUSEGUI )
  {
    m_Gui = new mafGUI(NULL);
    m_Gui->SetListener(this);
    m_Gui->Reparent(this);
    m_PreviewSizer->Add(m_Gui,0,wxEXPAND);
  }
  Add(m_PreviewSizer,1);
}
//----------------------------------------------------------------------------
mafGUIDialogPreview::~mafGUIDialogPreview()
//----------------------------------------------------------------------------
{
  // this is the right place where to delete rwi.
  // the rest of the dialog is destructed by inherited 
  // destructor that execute after this.  //SIL. 20-4-2005: 
  cppDEL(m_Rwi);
}
