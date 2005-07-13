/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmdP5Glove.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-13 13:53:00 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmdP5Glove.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmdP5Glove.h"
#include "mflTransform.h"
#include "mflSmartPointer.h"
#include "mflEvent.h"
#include "mflDefines.h"
#include "P5dll.h"
#include "P5Motion.h"
#include "P5Bend.h"

#include "vtkMatrix4x4.h"

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
CP5DLL *mmdP5Glove::P5=NULL;

//------------------------------------------------------------------------------
//vtkStandardNewMacro(mmdP5Glove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmdP5Glove::mmdP5Glove()
//------------------------------------------------------------------------------
{
  this->SetThreaded(1);
  vtkNEW(TmpPose);
  IndexValue  = 0;
  MiddleValue = 0;
  RingValue   = 0;
  PinkyValue  = 0;
  ThumbValue  = 0;
  IndexSensitivity  = 0;
  MiddleSensitivity = 0;
  RingSensitivity   = 0;
  PinkySensitivity  = 0;
  ThumbSensitivity  = 0;
  SetNumberOfButtons(5);
}

//------------------------------------------------------------------------------
mmdP5Glove::~mmdP5Glove()
//------------------------------------------------------------------------------
{
  vtkDEL(TmpPose);
}

//------------------------------------------------------------------------------
int mmdP5Glove::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (P5==NULL)
  {
    P5 = new CP5DLL;
  }

  if (!P5->P5_Init())
	{
	  vtkErrorMacro("P5 Tracker Not Found");
    delete P5;
    P5=NULL;
    return -1;
	}
	else
	{
    if (P5->m_nNumP5==0)
    {
      vtkErrorMacro("0 gloves connected to P5, shutdown...");
      P5->P5_Close();
      delete P5;
      P5=NULL;
      return -1;
    }
    
		RECT region;

		P5Motion_Init(P5, 0);

		P5Motion_InvertMouse(P5MOTION_INVERTAXIS, P5MOTION_NORMALAXIS, P5MOTION_NORMALAXIS);
#ifdef WIN32
    GetClipCursor(&region);
		P5Motion_SetClipRegion(region.left, region.right, region.top, region.bottom, region.top, region.bottom);
#endif

    // initilize the bend utility routine
    P5Bend_Init(P5, 0);

    // set sensitivity (notice P5 lib has a global sensitivity variable)
    SetThumbSensitivity(nBendSensitivity[P5_THUMB]);
	  SetIndexSensitivity(nBendSensitivity[P5_INDEX]);
	  SetMiddleSensitivity(nBendSensitivity[P5_MIDDLE]);
	  SetRingSensitivity(nBendSensitivity[P5_RING]);
	  SetPinkySensitivity(nBendSensitivity[P5_PINKY]);

    // disable Mouse modality for P5 0
		P5->P5_SetMouseState(0, FALSE);

    vtkGenericWarningMacro("P5 Tracker Found & Initialized")
	}

  return (this->Superclass::InternalInitialize());
}

//------------------------------------------------------------------------------
void mmdP5Glove::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown(); // stop thread
  if (P5)
  {
    P5->P5_Close();
    delete P5;
    P5=NULL;
  }
}

//------------------------------------------------------------------------------
int mmdP5Glove::InternalUpdate()
//------------------------------------------------------------------------------
{ 
  //float xyz[3];
  //float pyr[3];

	if( P5 && P5->m_P5Devices != NULL)
	{
    
    P5Data glove = P5->m_P5Devices[0];
    P5Motion_Process();

    TmpPose->Identity();
    mflTransform::SetPosition(TmpPose,fFilterX, fFilterY, -fFilterZ);
    
    // todo: to be checked
    mflTransform::RotateZ(TmpPose,-fAbsRollPos, PRE_MULTIPLY);
    mflTransform::RotateX(TmpPose,fAbsPitchPos, PRE_MULTIPLY);
    mflTransform::RotateY(TmpPose,-fAbsYawPos, PRE_MULTIPLY);
		
    
    // the matrix is automatically copied and timestamped
    this->SetLastPoseMatrix(TmpPose);
    
    unsigned char value;

    value=((unsigned char)glove.m_byBendSensor_Data[P5_INDEX]);
		IndexValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_MIDDLE]);
		MiddleValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_RING]);
		RingValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_PINKY]);
		PinkyValue = value;

		value=((unsigned char)glove.m_byBendSensor_Data[P5_THUMB]);
		ThumbValue = value;

    // update bend sensors sensitivity
		P5Bend_SetClickSensitivity(P5_THUMB, ThumbSensitivity);
		P5Bend_SetClickSensitivity(P5_INDEX, IndexSensitivity);
		P5Bend_SetClickSensitivity(P5_MIDDLE, MiddleSensitivity);
		P5Bend_SetClickSensitivity(P5_RING, RingSensitivity);
		P5Bend_SetClickSensitivity(P5_PINKY, PinkySensitivity);

		P5Bend_Process();

    SetButtonState(0,bP5ClickLevel[P5_INDEX]);
    SetButtonState(1,bP5ClickLevel[P5_MIDDLE]);
    SetButtonState(2,bP5ClickLevel[P5_RING]);
    SetButtonState(3,bP5ClickLevel[P5_PINKY]);
    SetButtonState(4,bP5ClickLevel[P5_THUMB]);
    
    Sleep(25);
    
	  return 0;
	}
	else
  {
	    return -1; // stop UpdateLoop
  }
}
