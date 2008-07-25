/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUILab.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
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


#include "mafGUILab.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUILab,wxStaticText)
    EVT_LEFT_DOWN(mafGUILab::OnLeftMouseButtonDown)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mafGUILab,wxStaticText)

//----------------------------------------------------------------------------
void mafGUILab::OnLeftMouseButtonDown(wxMouseEvent &event)
//----------------------------------------------------------------------------
{
  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  e.SetEventObject(this);
  ProcessEvent(e);
}
