/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMeter
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

#include "albaVMEOutputMeter.h"
#include "albaVMEMeter.h"
#include "albaGUI.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputMeter::albaVMEOutputMeter()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEOutputMeter::~albaVMEOutputMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
albaGUI *albaVMEOutputMeter::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();
  
  m_Distance = ((albaVMEMeter *)m_VME)->GetDistance();
  m_Gui->Label(_("distance: "), &m_Distance, true);

  m_Angle = ((albaVMEMeter *)m_VME)->GetAngle();
  m_Gui->Label(_("angle: "), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputMeter::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  if(((albaVMEMeter *)m_VME)->GetMeterMode() == albaVMEMeter::POINT_DISTANCE || ((albaVMEMeter *)m_VME)->GetMeterMode() == albaVMEMeter::LINE_DISTANCE)
  {
  m_Distance = ((albaVMEMeter *)m_VME)->GetDistance();
  m_Angle ="";
  }
  else if(((albaVMEMeter *)m_VME)->GetMeterMode() == albaVMEMeter::LINE_ANGLE)
  {
    m_Distance ="";
    m_Angle= ((albaVMEMeter *)m_VME)->GetAngle();
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
