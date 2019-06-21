/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaInteractorPERTest.h"
#include "albaInteractorPER.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaVMESurface.h"
#include "vtkCamera.h"


void albaInteractorPERTest::TestFixture()
{

}

void albaInteractorPERTest::TestConstructorDestructor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  mip->Delete();
}

void albaInteractorPERTest::TestEnableSelect()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_CanSelect == true);
  mip->EnableSelect(false);
  CPPUNIT_ASSERT(mip->m_CanSelect == false);

  mip->Delete();
}

void albaInteractorPERTest::TestFlyToMode()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_FlyToFlag == false);
  mip->FlyToMode();
  CPPUNIT_ASSERT(mip->m_FlyToFlag == true);

  mip->Delete();
}

void albaInteractorPERTest::TestIsInteracting()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  bool isInteracting = mip->IsInteracting(dummyDevice);
  CPPUNIT_ASSERT(isInteracting == false);
  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestStartInteraction()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  
  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestStopInteraction()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);
  
  int stopped = mip->StopInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(stopped == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestOnEvent()
{
  // mostly to test for leaks
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  mip->OnEvent(&dummyMouse2DMoveEvent);
  mip->Delete();
}

void albaInteractorPERTest::TestOnVmeSelected()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaVMESurface *dummy = albaVMESurface::New();

  CPPUNIT_ASSERT(mip->m_VME == NULL);
  mip->OnVmeSelected(dummy);
  CPPUNIT_ASSERT(mip->m_VME == dummy);

  dummy->Delete();
  mip->Delete();
}

void albaInteractorPERTest::TestSetGetMode()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetMode(albaInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);

  mip->Delete();
}

void albaInteractorPERTest::TestSetModeToSingleButton()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetMode(albaInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);
  
  mip->SetModeToSingleButton();

  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->Delete();
}

void albaInteractorPERTest::TestSetModeToMultiButton()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::SINGLE_BUTTON);

  mip->SetModeToMultiButton();
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == albaInteractorPER::MULTI_BUTTON);

  mip->Delete();
}


void albaInteractorPERTest::TestGetCameraMouseInteractor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaInteractor *interactor = mip->GetCameraMouseInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

  mip->Delete();
}

void albaInteractorPERTest::TestGetCameraInteractor()
{
  albaInteractorPER *mip = albaInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  albaInteractor *interactor = mip->GetCameraInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

  mip->Delete();
}