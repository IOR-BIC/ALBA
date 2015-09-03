/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERTest
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
#include "mafInteractorPERTest.h"
#include "mafInteractorPER.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventInteraction.h"
#include "mafVMESurface.h"
#include "vtkCamera.h"


void mafInteractorPERTest::TestFixture()
{

}

void mafInteractorPERTest::TestConstructorDestructor()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  mip->Delete();
}

void mafInteractorPERTest::TestEnableSelect()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_CanSelect == true);
  mip->EnableSelect(false);
  CPPUNIT_ASSERT(mip->m_CanSelect == false);

  mip->Delete();
}

void mafInteractorPERTest::TestFlyToMode()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  CPPUNIT_ASSERT(mip->m_FlyToFlag == false);
  mip->FlyToMode();
  CPPUNIT_ASSERT(mip->m_FlyToFlag == true);

  mip->Delete();
}

void mafInteractorPERTest::TestIsInteracting()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();

  bool isInteracting = mip->IsInteracting(dummyDevice);
  CPPUNIT_ASSERT(isInteracting == false);
  dummyDevice->Delete();
  mip->Delete();
}

void mafInteractorPERTest::TestStartInteraction()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);
  
  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void mafInteractorPERTest::TestStopInteraction()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();

  int started = mip->StartInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(started == 1);
  
  int stopped = mip->StopInteraction(dummyDevice, -1);
  CPPUNIT_ASSERT(stopped == 1);

  dummyDevice->Delete();
  mip->Delete();
}

void mafInteractorPERTest::TestOnEvent()
{
  // mostly to test for leaks
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(mafDeviceButtonsPadMouse::GetMouse2DMoveId());

  mip->OnEvent(&dummyMouse2DMoveEvent);
  mip->Delete();
  CPPUNIT_ASSERT(true); 
}

void mafInteractorPERTest::TestOnVmeSelected()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafVMESurface *dummy = mafVMESurface::New();

  CPPUNIT_ASSERT(mip->m_VME == NULL);
  mip->OnVmeSelected(dummy);
  CPPUNIT_ASSERT(mip->m_VME == dummy);

  dummy->Delete();
  mip->Delete();
}

void mafInteractorPERTest::TestSetGetMode()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::SINGLE_BUTTON);

  mip->SetMode(mafInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::MULTI_BUTTON);

  mip->Delete();
}

void mafInteractorPERTest::TestSetModeToSingleButton()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::SINGLE_BUTTON);

  mip->SetMode(mafInteractorPER::MULTI_BUTTON);
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::MULTI_BUTTON);
  
  mip->SetModeToSingleButton();

  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::SINGLE_BUTTON);

  mip->Delete();
}

void mafInteractorPERTest::TestSetModeToMultiButton()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  int mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::SINGLE_BUTTON);

  mip->SetModeToMultiButton();
  mode = mip->GetMode();
  CPPUNIT_ASSERT(mode == mafInteractorPER::MULTI_BUTTON);

  mip->Delete();
}

void mafInteractorPERTest::TestLinkCameraAdd()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  vtkCamera *dummyCamera = vtkCamera::New(); 
  mip->LinkCameraAdd(dummyCamera);
  dummyCamera->Delete();
  mip->Delete();

  CPPUNIT_ASSERT(true);
}

void mafInteractorPERTest::TestLinkCameraRemove()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  vtkCamera *dummyCamera = vtkCamera::New(); 
  mip->LinkCameraAdd(dummyCamera);
  mip->LinkCameraRemove(dummyCamera);
  dummyCamera->Delete();
  mip->Delete();

  CPPUNIT_ASSERT(true);
}

void mafInteractorPERTest::TestLinkCameraRemoveAll()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  vtkCamera *dummyCamera = vtkCamera::New(); 
  mip->LinkCameraAdd(dummyCamera);

  vtkCamera *dummyCamera2 = vtkCamera::New(); 
  mip->LinkCameraAdd(dummyCamera2);

  mip->LinkCameraRemoveAll();
  dummyCamera2->Delete();
  dummyCamera->Delete();
  mip->Delete();

  CPPUNIT_ASSERT(true);

}

void mafInteractorPERTest::TestGetCameraMouseInteractor()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafInteractor *interactor = mip->GetCameraMouseInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

  mip->Delete();

  CPPUNIT_ASSERT(true);
}

void mafInteractorPERTest::TestGetCameraInteractor()
{
  mafInteractorPER *mip = mafInteractorPER::New();
  CPPUNIT_ASSERT(mip);

  mafInteractor *interactor = mip->GetCameraInteractor();
  CPPUNIT_ASSERT(interactor->GetDevice() == NULL);

  mip->Delete();
}