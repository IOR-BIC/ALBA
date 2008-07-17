/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceTrackerWIIMote.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-17 16:01:04 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mafDeviceTrackerWIIMote.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafEvent.h"

//include wiimote drivers
#include "Console.h"
/*#include "Utils.h"
#include "Wiimote.h"*/

#include "mafMatrix.h"

mafCxxTypeMacro(mafDeviceTrackerWIIMote)


//------------------------------------------------------------------------------
//vtkStandardNewMacro(mafDeviceTrackerWIIMote)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDeviceTrackerWIIMote::mafDeviceTrackerWIIMote()
//------------------------------------------------------------------------------
{
  this->SetThreaded(1);
  mafNEW(m_TmpPose);
  
	m_XTranslation = m_YTranslation = m_ZTranslation = 0.;
	m_TranslationCalibration = 1.;
}

//------------------------------------------------------------------------------
mafDeviceTrackerWIIMote::~mafDeviceTrackerWIIMote()
//------------------------------------------------------------------------------
{
  mafDEL(m_TmpPose);
}

//------------------------------------------------------------------------------
int mafDeviceTrackerWIIMote::InternalInitialize()
//------------------------------------------------------------------------------
{
  wiimotes = HIDDevice::ConnectToHIDDevices<Wiimote>();

	if (wiimotes.size() <= 0) // Need to add a user fri
		return 1;


	wiimotes[0].SetLEDs(0, 1, 1, 0);
	wiimotes[0].StartListening();
	wiimotes[0].RequestMotionData();

  return (this->Superclass::InternalInitialize());
	
}

//------------------------------------------------------------------------------
void mafDeviceTrackerWIIMote::InternalShutdown()
//------------------------------------------------------------------------------
{
	if(wiimotes.size() > 0)
	{
		wiimotes[0].StopListening();
		wiimotes[0].Disconnect();

		wiimotes.clear();
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
int mafDeviceTrackerWIIMote::InternalUpdate()
//------------------------------------------------------------------------------
{ 

	if(wiimotes.size() > 0)
	{
		MotionData p = wiimotes[0].GetLastMotionData();
		Button & buttonA = wiimotes[0].GetButton("A");
		Button & buttonB = wiimotes[0].GetButton("B");
		//Button & buttonC = wiimotes[0].GetButton("C");
		//Button & buttonZ = wiimotes[0].GetButton("Z");
		

		mafLogMessage("%d %d %d - button : %s %s" , p.x, p.y, p.z, \
			buttonA.Pressed()?"pressedA":"", buttonB.Pressed()?"pressedB":"");

		/*
		//with nunchuck
		mafLogMessage("%d %d %d - button : %s %s %s %s" , p.x, p.y, p.z, \
			            buttonA.Pressed()?"pressedA":"", buttonB.Pressed()?"pressedB":"",buttonC.Pressed()?"pressedC":"",buttonZ.Pressed()?"pressedZ":"");
    */

		m_TranslationCalibration =  .1;
		
		
		SetTranlationAndRotation(p);
		
		
		m_TmpPose->Identity();
		mafTransform::SetPosition(*m_TmpPose,m_XTranslation, m_YTranslation, m_ZTranslation);

		// todo: to be checked
		/*mafTransform::RotateZ(*m_TmpPose,-fAbsRollPos, PRE_MULTIPLY);
		mafTransform::RotateX(*m_TmpPose,fAbsPitchPos, PRE_MULTIPLY);
		mafTransform::RotateY(*m_TmpPose,-fAbsYawPos, PRE_MULTIPLY);*/

		mafLogMessage("MATRIX");
		mafLogMessage("%.2f", m_TmpPose->GetElement(0,3));
		mafLogMessage("%.2f", m_TmpPose->GetElement(1,3));
		mafLogMessage("%.2f", m_TmpPose->GetElement(2,3));
		mafLogMessage("END MATRIX");
    
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
void mafDeviceTrackerWIIMote::SetTranlationAndRotation(MotionData p)
//------------------------------------------------------------------------------
{
	if (last_positions.size() <= 2)
	{	
		last_positions.push_back(p);
		return;
	}

	last_positions.erase(last_positions.begin());

	/*if (p.y > 30) p.y = 30; // Restrict the values to -30 to 30
	if (p.y < -30) p.y = -30;*/

	last_positions.push_back(p);

	m_XTranslation = (last_positions[1].x - last_positions[0].x) * m_TranslationCalibration;
	m_YTranslation = (last_positions[1].y - last_positions[0].y) * m_TranslationCalibration;
	m_ZTranslation = (last_positions[1].z - last_positions[0].z) * m_TranslationCalibration;

	m_Angle = m_TranslationCalibration * (last_positions[0].x + last_positions[1].x + last_positions[2].x) / 3.0;
}
//------------------------------------------------------------------------------
void mafDeviceTrackerWIIMote::Calibration()
//------------------------------------------------------------------------------
{

}
