/*=========================================================================

Program: MAF2Medical
Module: medGUIMDIFrame
Authors: Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "medDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafEvent.h"
#include "medGUIMDIFrame.h"



//----------------------------------------------------------------------------
// mafGUIMDIFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(medGUIMDIFrame, mafGUIMDIFrame)
		EVT_MENU_RANGE(WIZARD_START,WIZARD_END,medGUIMDIFrame::OnMenuWizard)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
medGUIMDIFrame::medGUIMDIFrame(const wxString& title, const wxPoint& pos, const wxSize& size):mafGUIMDIFrame(title, pos, size)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
medGUIMDIFrame::~medGUIMDIFrame()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medGUIMDIFrame::OnMenuWizard(wxCommandEvent& e)
//----------------------------------------------------------------------------
{ 
  mafEventMacro(mafEvent(this,MENU_WIZARD,(long)e.GetId()));
}
