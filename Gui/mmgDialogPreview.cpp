/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialogPreview.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-20 14:22:21 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
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

#include "mmgDialogPreview.h"
#include "mafDecl.h"
#include "mafRWI.h"

//----------------------------------------------------------------------------
mmgDialogPreview::mmgDialogPreview(const wxString& title,long style)
: mmgDialog(title, style)
//----------------------------------------------------------------------------
{
  m_preview_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_rwi = NULL;
  m_gui = NULL;

  if( style & mafUSERWI )
  {
    m_rwi = new mafRWI(this);
    m_rwi->SetSize(0,0,300,200);
    m_rwi->Show(true);
    m_preview_sizer->Add(m_rwi->m_rwi,1,wxEXPAND);
  }
  if( style & mafUSEGUI )
  {
    m_gui = new mmgGui(NULL);
    m_gui->SetListener(this);
    m_gui->Reparent(this);
    m_preview_sizer->Add(m_gui,0,wxEXPAND);
  }
  Add(m_preview_sizer,1);
}
//----------------------------------------------------------------------------
mmgDialogPreview::~mmgDialogPreview()
//----------------------------------------------------------------------------
{
  // this is the right place where to delete rwi.
  // the rest of the dialog is destructed by inherited 
  // destructor that execute after this.  //SIL. 20-4-2005: 
  cppDEL(m_rwi);
}
