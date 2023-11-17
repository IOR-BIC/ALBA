/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
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

#include "albaVMEOutputComputeWrapping.h"
#include "albaVMEComputeWrapping.h"
#include "albaGUI.h"

#include <assert.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEOutputComputeWrapping)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutputComputeWrapping::albaVMEOutputComputeWrapping()
//-------------------------------------------------------------------------
{
	m_MiddlePoints.push_back(albaString()); //first middlepoint
	m_MiddlePoints.push_back(albaString()); //last middlepoint
}

//-------------------------------------------------------------------------
albaVMEOutputComputeWrapping::~albaVMEOutputComputeWrapping()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
albaGUI *albaVMEOutputComputeWrapping::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaVMEOutput::CreateGui();

  albaVMEComputeWrapping *wrappedMeter = (albaVMEComputeWrapping *)m_VME;
  m_Distance = wrappedMeter->GetDistance();
  m_Gui->Label(_("Distance:"), &m_Distance, true);

	double *coordinateFIRST = NULL;
	double *coordinateLAST = NULL;

 /* if(wrappedMeter->GetWrappedMode() == medVMEComputeWrapping::MANUAL_WRAP)
  {
    coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
    coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
  }
  else /*if(wrappedMeter->GetWrappedMode() == medVMEComputeWrapping::AUTOMATED_WRAP)
  {
    coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
    coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
  }*/

  if(coordinateFIRST != NULL)
    m_MiddlePoints[0] = albaString::Format("%.2f %.2f %.2f", coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
  if(coordinateLAST != NULL)
    m_MiddlePoints[m_MiddlePoints.size()-1] = albaString::Format("%.2f %.2f %.2f", coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

  
	m_Gui->Label(_("First mp:"), &m_MiddlePoints[0], true);
	m_Gui->Label(_("Last mp:"), &m_MiddlePoints[m_MiddlePoints.size()-1], true);

  m_Angle = wrappedMeter->GetAngle();
  m_Gui->Label(_("Angle:"), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEOutputComputeWrapping::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  albaVMEComputeWrapping *wrappedMeter = (albaVMEComputeWrapping *)m_VME;

  if(wrappedMeter->GetMeterMode() == albaVMEComputeWrapping::POINT_DISTANCE)
  {
		m_Distance = ((albaVMEComputeWrapping *)m_VME)->GetDistance();

    double *coordinateFIRST = NULL;
    double *coordinateLAST = NULL; 
    
    if(wrappedMeter->GetNumberMiddlePoints() == 0) return;
    /*
	if(wrappedMeter->GetWrappedMode() == medVMEComputeWrapping::MANUAL_WRAP)
    {
      coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
      coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
    }
    else /*if(wrappedMeter->GetWrappedMode() == medVMEComputeWrapping::AUTOMATED_WRAP)
    {
      coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
      coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
    }*/

		
		if(coordinateFIRST != NULL)
			m_MiddlePoints[0] = albaString::Format("%.2f %.2f %.2f", coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
		if(coordinateLAST != NULL)
			m_MiddlePoints[m_MiddlePoints.size()-1] = albaString::Format("%.2f %.2f %.2f", coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

		m_Angle ="";
  }
/*	else if(wrappedMeter->GetMeterMode() == medVMEComputeWrapping::LINE_DISTANCE)
	{
		m_Distance = wrappedMeter->GetDistance();
		m_Angle ="";
	}
  else if(wrappedMeter->GetMeterMode() == medVMEComputeWrapping::LINE_ANGLE)
  {
    m_Distance ="";
    m_Angle= wrappedMeter->GetAngle();
  }*/
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
