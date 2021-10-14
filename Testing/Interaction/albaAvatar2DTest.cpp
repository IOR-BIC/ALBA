/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar2DTest
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
#include "albaAvatar2DTest.h"
#include "albaAvatar2D.h"
#include "albaSmartPointer.h"
#include "albaDeviceButtonsPadMouse.h"
#include "vtkRenderer.h"


void albaAvatar2DTest::TestFixture()
{

}

void albaAvatar2DTest::TestConstructorDestructor()
{
  albaAvatar2D *avatar = NULL;
  avatar = albaAvatar2D::New();
  CPPUNIT_ASSERT(avatar);
  avatar->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  albaSmartPointer<albaAvatar2D> avatarSmartPointer;
}

void albaAvatar2DTest::TestOnEvent()
{
  albaAvatar2D *avatar = NULL;
  avatar = albaAvatar2D::New();
  CPPUNIT_ASSERT(avatar);
  CPPUNIT_ASSERT(avatar->GetRenderer() == NULL);

  albaEventBase dummyEventNotHandledByTestedClass;
  dummyEventNotHandledByTestedClass.SetSender(this);

  avatar->OnEvent(&dummyEventNotHandledByTestedClass);

  vtkRenderer *dummyRenderer = vtkRenderer::New();
  vtkRenderer *dummyRendererBackup = dummyRenderer;

  albaEvent dummyEventHandledByTestedClass;
  dummyEventHandledByTestedClass.SetSender(this);
  dummyEventHandledByTestedClass.SetId(VIEW_SELECT);
  dummyEventHandledByTestedClass.SetData(dummyRenderer);
  
  avatar->OnEvent(&dummyEventHandledByTestedClass);

  CPPUNIT_ASSERT(avatar->GetRenderer() == dummyRenderer);
  avatar->Delete();
  
  CPPUNIT_ASSERT(dummyRendererBackup == dummyRenderer);
  dummyRenderer->Delete();
}

void albaAvatar2DTest::TestSetMouse()
{
  albaAvatar2D *avatar = NULL;
  avatar = albaAvatar2D::New();
  CPPUNIT_ASSERT(avatar);

  CPPUNIT_ASSERT(avatar->m_Device == NULL);
  
  albaDeviceButtonsPadMouse *dummyMouse = albaDeviceButtonsPadMouse::New();
  albaDeviceButtonsPadMouse *dummyMouseBackup = dummyMouse;

  avatar->SetMouse(dummyMouse);

  CPPUNIT_ASSERT(avatar->m_Device == dummyMouse);

  avatar->Delete();

  CPPUNIT_ASSERT(dummyMouse == dummyMouseBackup);

  dummyMouse->Delete();

}

void albaAvatar2DTest::TestGetMouse()
{
  albaAvatar2D *avatar = NULL;
  avatar = albaAvatar2D::New();
  CPPUNIT_ASSERT(avatar);

  CPPUNIT_ASSERT(avatar->m_Device == NULL);
  CPPUNIT_ASSERT(avatar->GetMouse() == NULL);

  albaDeviceButtonsPadMouse *dummyMouse = albaDeviceButtonsPadMouse::New();
  albaDeviceButtonsPadMouse *dummyMouseBackup = dummyMouse;

  avatar->SetMouse(dummyMouse);

  CPPUNIT_ASSERT(avatar->m_Device == dummyMouse);
  CPPUNIT_ASSERT(avatar->GetMouse() == dummyMouse);

  avatar->Delete();

  CPPUNIT_ASSERT(dummyMouse == dummyMouseBackup);

  dummyMouse->Delete();
}
