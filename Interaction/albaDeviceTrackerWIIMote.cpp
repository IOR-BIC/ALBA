/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceTrackerWIIMote
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"

#include "albaDeviceTrackerWIIMote.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaEvent.h"

//include wiimote drivers
#include "Console.h"
/*#include "Utils.h"
#include "Wiimote.h"*/

#include "albaMatrix.h"

albaCxxTypeMacro(albaDeviceTrackerWIIMote)


//------------------------------------------------------------------------------
//vtkStandardNewMacro(albaDeviceTrackerWIIMote)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDeviceTrackerWIIMote::albaDeviceTrackerWIIMote()
//------------------------------------------------------------------------------
{
  this->SetThreaded(1);
  albaNEW(m_TmpPose);
  
	m_XTranslation = m_YTranslation = m_ZTranslation = 0.;
	m_TranslationCalibration = 1.;
}

//------------------------------------------------------------------------------
albaDeviceTrackerWIIMote::~albaDeviceTrackerWIIMote()
//------------------------------------------------------------------------------
{
  albaDEL(m_TmpPose);
}

//------------------------------------------------------------------------------
int albaDeviceTrackerWIIMote::InternalInitialize()
//------------------------------------------------------------------------------
{
  m_Wiimotes = HIDDevice::ConnectToHIDDevices<Wiimote>();

	if (m_Wiimotes.size() <= 0) // Need to add a user fri
		return 1;


	m_Wiimotes[0].SetLEDs(0, 1, 1, 0);
	m_Wiimotes[0].StartListening();
	m_Wiimotes[0].RequestMotionData();

  return (this->Superclass::InternalInitialize());
	
}

//------------------------------------------------------------------------------
void albaDeviceTrackerWIIMote::InternalShutdown()
//------------------------------------------------------------------------------
{
	if(m_Wiimotes.size() > 0)
	{
		m_Wiimotes[0].StopListening();
		m_Wiimotes[0].Disconnect();

		m_Wiimotes.clear();
	}

  Superclass::InternalShutdown(); // stop thread
  /*if (m_P5)
  {
    m_P5->P5_Close();
    delete m_P5;
    m_P5=NULL;
  }*/
}

//------------------------------------------------------------------------------
int albaDeviceTrackerWIIMote::InternalUpdate()
//------------------------------------------------------------------------------
{ 

	if(m_Wiimotes.size() > 0)
	{
		MotionData p = m_Wiimotes[0].GetLastMotionData();
		Button & buttonA = m_Wiimotes[0].GetButton("A");
		Button & buttonB = m_Wiimotes[0].GetButton("B");
		//Button & buttonC = m_Wiimotes[0].GetButton("C");
		//Button & buttonZ = m_Wiimotes[0].GetButton("Z");
		

		albaLogMessage("%d %d %d - button : %s %s" , p.x, p.y, p.z, \
			buttonA.Pressed()?"pressedA":"", buttonB.Pressed()?"pressedB":"");

		/*
		//with nunchuck
		albaLogMessage("%d %d %d - button : %s %s %s %s" , p.x, p.y, p.z, \
			            buttonA.Pressed()?"pressedA":"", buttonB.Pressed()?"pressedB":"",buttonC.Pressed()?"pressedC":"",buttonZ.Pressed()?"pressedZ":"");
    */

		m_TranslationCalibration =  .1;
		
		
		SetTranlationAndRotation(p);
		
		
		m_TmpPose->Identity();
		albaTransform::SetPosition(*m_TmpPose,m_XTranslation, m_YTranslation, m_ZTranslation);

		// todo: to be checked
		/*albaTransform::RotateZ(*m_TmpPose,-fAbsRollPos, PRE_MULTIPLY);
		albaTransform::RotateX(*m_TmpPose,fAbsPitchPos, PRE_MULTIPLY);
		albaTransform::RotateY(*m_TmpPose,-fAbsYawPos, PRE_MULTIPLY);*/

		albaLogMessage("MATRIX");
		albaLogMessage("%.2f", m_TmpPose->GetElement(0,3));
		albaLogMessage("%.2f", m_TmpPose->GetElement(1,3));
		albaLogMessage("%.2f", m_TmpPose->GetElement(2,3));
		albaLogMessage("END MATRIX");
    
		// the matrix is automatically copied and timestamped
		SetLastPoseMatrix(*m_TmpPose);

		Sleep(25);
		return 0;
	}
	else
	{
		return -1;
	}
}
//------------------------------------------------------------------------------
void albaDeviceTrackerWIIMote::SetTranlationAndRotation(MotionData p)
//------------------------------------------------------------------------------
{
	if (m_LastPositions.size() <= 2)
	{	
		m_LastPositions.push_back(p);
		return;
	}

	m_LastPositions.erase(m_LastPositions.begin());

	/*if (p.y > 30) p.y = 30; // Restrict the values to -30 to 30
	if (p.y < -30) p.y = -30;*/

	m_LastPositions.push_back(p);

	m_XTranslation = (m_LastPositions[1].x - m_LastPositions[0].x) * m_TranslationCalibration;
	m_YTranslation = (m_LastPositions[1].y - m_LastPositions[0].y) * m_TranslationCalibration;
	m_ZTranslation = (m_LastPositions[1].z - m_LastPositions[0].z) * m_TranslationCalibration;

	m_Angle = m_TranslationCalibration * (m_LastPositions[0].x + m_LastPositions[1].x + m_LastPositions[2].x) / 3.0;
}
//------------------------------------------------------------------------------
void albaDeviceTrackerWIIMote::Calibration()
//------------------------------------------------------------------------------
{

}
