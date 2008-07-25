/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.8 $
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
#include "mafGUI.h"

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
mafGUI *mafVMEOutputMeter::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  
  m_Distance = ((mafVMEMeter *)m_VME)->GetDistance();
  m_Gui->Label(_("distance: "), &m_Distance, true);

  m_Angle = ((mafVMEMeter *)m_VME)->GetAngle();
  m_Gui->Label(_("angle: "), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputMeter::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  if(((mafVMEMeter *)m_VME)->GetMeterMode() == mafVMEMeter::POINT_DISTANCE || ((mafVMEMeter *)m_VME)->GetMeterMode() == mafVMEMeter::LINE_DISTANCE)
  {
  m_Distance = ((mafVMEMeter *)m_VME)->GetDistance();
  m_Angle ="";
  }
  else if(((mafVMEMeter *)m_VME)->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
  {
    m_Distance ="";
    m_Angle= ((mafVMEMeter *)m_VME)->GetAngle();
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
