/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEOutputWrappedMeter.h"
#include "mafVMEWrappedMeter.h"
#include "mafGUI.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputWrappedMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputWrappedMeter::mafVMEOutputWrappedMeter()
//-------------------------------------------------------------------------
{
	m_MiddlePoints.push_back(mafString()); //first middlepoint
	m_MiddlePoints.push_back(mafString()); //last middlepoint
}

//-------------------------------------------------------------------------
mafVMEOutputWrappedMeter::~mafVMEOutputWrappedMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafGUI *mafVMEOutputWrappedMeter::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  mafVMEWrappedMeter *wrappedMeter = (mafVMEWrappedMeter *)m_VME;
  m_Distance = wrappedMeter->GetDistance();
  m_Gui->Label(_("distance: "), &m_Distance, true);

	double *coordinateFIRST = NULL;
	double *coordinateLAST = NULL;

  if(wrappedMeter->GetWrappedMode() == mafVMEWrappedMeter::MANUAL_WRAP)
  {
    coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
    coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
  }
  else /*if(wrappedMeter->GetWrappedMode() == mafVMEWrappedMeter::AUTOMATED_WRAP)*/
  {
    coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
    coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
  }

  if(coordinateFIRST != NULL)
    m_MiddlePoints[0] = wxString::Format("%.2f %.2f %.2f", coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
  if(coordinateLAST != NULL)
    m_MiddlePoints[m_MiddlePoints.size()-1] = wxString::Format("%.2f %.2f %.2f", coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

  
	m_Gui->Label(_("first mp:"), &m_MiddlePoints[0], true);
	m_Gui->Label(_("last mp:"), &m_MiddlePoints[m_MiddlePoints.size()-1], true);

  m_Angle = wrappedMeter->GetAngle();
  m_Gui->Label(_("angle: "), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputWrappedMeter::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  mafVMEWrappedMeter *wrappedMeter = (mafVMEWrappedMeter *)m_VME;

  if(wrappedMeter->GetMeterMode() == mafVMEWrappedMeter::POINT_DISTANCE)
  {
		m_Distance = ((mafVMEWrappedMeter *)m_VME)->GetDistance();

    double *coordinateFIRST = NULL;
    double *coordinateLAST = NULL; 
    
    if(wrappedMeter->GetNumberMiddlePoints() == 0) return;
    if(wrappedMeter->GetWrappedMode() == mafVMEWrappedMeter::MANUAL_WRAP)
    {
      coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
      coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
    }
    else /*if(wrappedMeter->GetWrappedMode() == mafVMEWrappedMeter::AUTOMATED_WRAP)*/
    {
      coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
      coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
    }

		
		if(coordinateFIRST != NULL)
			m_MiddlePoints[0] = wxString::Format("%.2f %.2f %.2f", coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
		if(coordinateLAST != NULL)
			m_MiddlePoints[m_MiddlePoints.size()-1] = wxString::Format("%.2f %.2f %.2f", coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

		m_Angle ="";
  }
/*	else if(wrappedMeter->GetMeterMode() == mafVMEWrappedMeter::LINE_DISTANCE)
	{
		m_Distance = wrappedMeter->GetDistance();
		m_Angle ="";
	}
  else if(wrappedMeter->GetMeterMode() == mafVMEWrappedMeter::LINE_ANGLE)
  {
    m_Distance ="";
    m_Angle= wrappedMeter->GetAngle();
  }*/
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
