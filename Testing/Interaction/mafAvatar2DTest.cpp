/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAvatar2DTest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-27 07:06:01 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafAvatar2DTest.h"
#include "mafAvatar2D.h"
#include "mafSmartPointer.h"
#include "mafDeviceButtonsPadMouse.h"
#include "vtkRenderer.h"

void mafAvatar2DTest::setUp()
{

}

void mafAvatar2DTest::tearDown()
{

}

void mafAvatar2DTest::TestFixture()
{

}

void mafAvatar2DTest::TestConstructorDestructor()
{
  mafAvatar2D *avatar = NULL;
  avatar = mafAvatar2D::New();
  CPPUNIT_ASSERT(avatar);
  avatar->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  mafSmartPointer<mafAvatar2D> avatarSmartPointer;
}

void mafAvatar2DTest::TestOnEvent()
{
  mafAvatar2D *avatar = NULL;
  avatar = mafAvatar2D::New();
  CPPUNIT_ASSERT(avatar);
  CPPUNIT_ASSERT(avatar->GetRenderer() == NULL);

  mafEventBase dummyEventNotHandledByTestedClass;
  dummyEventNotHandledByTestedClass.SetSender(this);

  avatar->OnEvent(&dummyEventNotHandledByTestedClass);

  vtkRenderer *dummyRenderer = vtkRenderer::New();
  vtkRenderer *dummyRendererBackup = dummyRenderer;

  mafEventBase dummyEventHandledByTestedClass;
  dummyEventHandledByTestedClass.SetSender(this);
  dummyEventHandledByTestedClass.SetId(VIEW_SELECT);
  dummyEventHandledByTestedClass.SetData(dummyRenderer);
  
  avatar->OnEvent(&dummyEventHandledByTestedClass);

  CPPUNIT_ASSERT(avatar->GetRenderer() == dummyRenderer);
  avatar->Delete();
  
  CPPUNIT_ASSERT(dummyRendererBackup == dummyRenderer);
  dummyRenderer->Delete();
}

void mafAvatar2DTest::TestSetMouse()
{
  mafAvatar2D *avatar = NULL;
  avatar = mafAvatar2D::New();
  CPPUNIT_ASSERT(avatar);

  CPPUNIT_ASSERT(avatar->m_Device == NULL);
  
  mafDeviceButtonsPadMouse *dummyMouse = mafDeviceButtonsPadMouse::New();
  mafDeviceButtonsPadMouse *dummyMouseBackup = dummyMouse;

  avatar->SetMouse(dummyMouse);

  CPPUNIT_ASSERT(avatar->m_Device == dummyMouse);

  avatar->Delete();

  CPPUNIT_ASSERT(dummyMouse == dummyMouseBackup);

  dummyMouse->Delete();

}

void mafAvatar2DTest::TestGetMouse()
{
  mafAvatar2D *avatar = NULL;
  avatar = mafAvatar2D::New();
  CPPUNIT_ASSERT(avatar);

  CPPUNIT_ASSERT(avatar->m_Device == NULL);
  CPPUNIT_ASSERT(avatar->GetMouse() == NULL);

  mafDeviceButtonsPadMouse *dummyMouse = mafDeviceButtonsPadMouse::New();
  mafDeviceButtonsPadMouse *dummyMouseBackup = dummyMouse;

  avatar->SetMouse(dummyMouse);

  CPPUNIT_ASSERT(avatar->m_Device == dummyMouse);
  CPPUNIT_ASSERT(avatar->GetMouse() == dummyMouse);

  avatar->Delete();

  CPPUNIT_ASSERT(dummyMouse == dummyMouseBackup);

  dummyMouse->Delete();
}
