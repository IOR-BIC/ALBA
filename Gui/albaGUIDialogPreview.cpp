/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDialogPreview
 Authors: Paolo Quadrani
 
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

#include "albaGUIDialogPreview.h"
#include "albaDecl.h"
#include "albaRWI.h"

//----------------------------------------------------------------------------
albaGUIDialogPreview::albaGUIDialogPreview(const wxString& title,long style)
: albaGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_PreviewSizer = new wxBoxSizer( wxHORIZONTAL );
  m_RwiSizer = new wxBoxSizer( wxVERTICAL );
  m_Rwi = NULL;
  m_Gui = NULL;

  if( style & albaUSERWI )
  {
    m_Rwi = new albaRWI(this);
    m_Rwi->SetSize(0,0,500,500);
    m_Rwi->Show(true);
    m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_PreviewSizer->Add(m_RwiSizer,1,wxEXPAND);
  }
  if( style & albaUSEGUI )
  {
    m_Gui = new albaGUI(NULL);
    m_Gui->SetListener(this);
    m_Gui->Reparent(this);
    m_PreviewSizer->Add(m_Gui,0,wxEXPAND);
  }
  Add(m_PreviewSizer,1);
}
//----------------------------------------------------------------------------
albaGUIDialogPreview::~albaGUIDialogPreview()
//----------------------------------------------------------------------------
{
  // this is the right place where to delete rwi.
  // the rest of the dialog is destructed by inherited 
  // destructor that execute after this.  //SIL. 20-4-2005: 
  cppDEL(m_Rwi);
}
