/*=========================================================================
Program:   VPH2
Module:    $RCSfile: medDeviceButtonsPadMouseDialog.cpp,v $
Language:  C++
Date:      $Date: 2009-07-15 15:23:35 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDeviceButtonsPadMouseDialog.h"

mafCxxTypeMacro(medDeviceButtonsPadMouseDialog)

//------------------------------------------------------------------------------
medDeviceButtonsPadMouseDialog::medDeviceButtonsPadMouseDialog()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
medDeviceButtonsPadMouseDialog::~medDeviceButtonsPadMouseDialog()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void medDeviceButtonsPadMouseDialog::SetView(mafView *view)
//------------------------------------------------------------------------------
{
  this->m_SelectedView=view;
}
