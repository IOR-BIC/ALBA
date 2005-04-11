/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgLab.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:22:23 $
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
