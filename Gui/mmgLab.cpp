/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLab.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:10:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmgLab.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mmgLab,wxStaticText)
    EVT_LEFT_DOWN(mmgLab::OnLeftMouseButtonDown)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mmgLab,wxStaticText)

//----------------------------------------------------------------------------
void mmgLab::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  e.SetEventObject(this);
  ProcessEvent(e);
}
