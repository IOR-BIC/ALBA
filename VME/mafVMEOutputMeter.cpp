/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:49:52 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEOutputMeter.h"
#include "mafVMEMeter.h"
#include "mmgGui.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputMeter::mafVMEOutputMeter()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputMeter::~mafVMEOutputMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mmgGui *mafVMEOutputMeter::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  m_Distance = ((mafVMEMeter *)m_VME)->GetDistance();
  m_Gui->Label("distance: ", m_Distance, true);
  return m_Gui;
}
