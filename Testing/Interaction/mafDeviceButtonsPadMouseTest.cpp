/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafDeviceButtonsPadMouseTest.h"
#include "mafDeviceButtonsPadMouse.h"

void mafDeviceButtonsPadMouseTest::setUp()
{

}

void mafDeviceButtonsPadMouseTest::tearDown()
{

}

void mafDeviceButtonsPadMouseTest::TestFixture()
{

}

void mafDeviceButtonsPadMouseTest::TestConstructorDestructor()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);
  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestSetGetLastPosition()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);
  
  double lastPosition[2] = {-9999,-9999};
  buttonsPadMouse->GetLastPosition(lastPosition);

  CPPUNIT_ASSERT(lastPosition[0] == 0.0 &&  lastPosition[1] == 0.0);
  
  double newLastPosition[2] = {-9999,-9999};
  buttonsPadMouse->SetLastPosition(newLastPosition[0],newLastPosition[1]);
  buttonsPadMouse->GetLastPosition(lastPosition);

  CPPUNIT_ASSERT(lastPosition[0] == newLastPosition[0] &&  lastPosition[1] == newLastPosition[1]);
  
  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestGetRenderer()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  vtkRenderer *renderer = NULL;
  renderer = buttonsPadMouse->GetRenderer();
  
  CPPUNIT_ASSERT(renderer == NULL);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestGetView()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  mafView *view = NULL;

  view = buttonsPadMouse->GetView();
  CPPUNIT_ASSERT(view == NULL);
  
  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestGetInteractor()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  vtkRenderWindowInteractor *vrwi = NULL;
  vrwi = buttonsPadMouse->GetInteractor();

  CPPUNIT_ASSERT(vrwi == NULL);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestGetRWI()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  mafRWIBase *rwi = NULL;
  rwi = buttonsPadMouse->GetRWI();

  CPPUNIT_ASSERT(rwi == NULL);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestSetUpdateRWIDuringMotion()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  bool updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == false);

  buttonsPadMouse->SetUpdateRWIDuringMotion(true);
  updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == true);
  
  buttonsPadMouse->SetUpdateRWIDuringMotion(false);
  updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == false);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestUpdateRWIDuringMotionOnOff()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  bool updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == false);

  buttonsPadMouse->UpdateRWIDuringMotionOn();
  updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == true);

  buttonsPadMouse->UpdateRWIDuringMotionOff();
  updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == false);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestIsUpdateRWIDuringMotion()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  bool updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == false);

  buttonsPadMouse->UpdateRWIDuringMotionOn();
  updateStatus = buttonsPadMouse->IsUpdateRWIDuringMotion();
  CPPUNIT_ASSERT(updateStatus == true);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestDisplayToNormalizedDisplay()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  double checkValues[2] = {10,20};
  double displayCoordinates[2] = {checkValues[0],checkValues[1]};
  buttonsPadMouse->DisplayToNormalizedDisplay(displayCoordinates);

  CPPUNIT_ASSERT(displayCoordinates[0] == checkValues[0] && displayCoordinates[1] == checkValues[1]);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestNormalizedDisplayToDisplay()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  double checkValues[2] = {10,20};
  double normalizedDisplayCoordinates[2] = {checkValues[0],checkValues[1]};
  buttonsPadMouse->NormalizedDisplayToDisplay(normalizedDisplayCoordinates);

  CPPUNIT_ASSERT(normalizedDisplayCoordinates[0] == checkValues[0] && normalizedDisplayCoordinates[1] == checkValues[1]);

  buttonsPadMouse->Delete();
}

void mafDeviceButtonsPadMouseTest::TestCollaborate()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);
  
  CPPUNIT_ASSERT(buttonsPadMouse->GetCollaborate() == false);

  buttonsPadMouse->Collaborate(true);
  CPPUNIT_ASSERT(buttonsPadMouse->GetCollaborate() == true);
  
  buttonsPadMouse->Collaborate(false);
  CPPUNIT_ASSERT(buttonsPadMouse->GetCollaborate() == false);

  buttonsPadMouse->Delete();
}

