/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorExtractIsosurfaceTest
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

#include "albaInteractorExtractIsosurfaceTest.h"
#include "albaInteractorExtractIsosurface.h"
#include "albaDeviceButtonsPadMouse.h" 
#include "albaEventInteraction.h"

void albaInteractorExtractIsosurfaceTest::TestFixture()
{

}

void albaInteractorExtractIsosurfaceTest::TestConstructor()
{
  albaInteractorExtractIsosurface *extractIsosurfaceInteractor = albaInteractorExtractIsosurface::New();
 
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_Renderer == NULL);

  extractIsosurfaceInteractor->Delete();
}

void albaInteractorExtractIsosurfaceTest::TestStartInteraction()
{
  albaDeviceButtonsPadMouse *buttonsPadMouse = albaDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  albaInteractorExtractIsosurface *extractIsosurfaceInteractor = albaInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  int returnValue = extractIsosurfaceInteractor->StartInteraction(buttonsPadMouse);
  
  // since no renderer exists this should return false
  CPPUNIT_ASSERT(returnValue == false);

  extractIsosurfaceInteractor->Delete();
  buttonsPadMouse->Delete();
}

void albaInteractorExtractIsosurfaceTest::TestOnMouseMove()
{
  albaInteractorExtractIsosurface *extractIsosurfaceInteractor = albaInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  extractIsosurfaceInteractor->OnMouseMove();

  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);

  extractIsosurfaceInteractor->Delete();
}

void albaInteractorExtractIsosurfaceTest::TestOnLeftButtonDown()
{
  albaEventInteraction *dummyEventInteraction = new albaEventInteraction;
  dummyEventInteraction->Set2DPosition(10,20);
  double dummyPosition[2];
  dummyEventInteraction->Get2DPosition(dummyPosition);
  CPPUNIT_ASSERT(dummyPosition[0] == 10 && dummyPosition[1] == 20);

  albaInteractorExtractIsosurface *extractIsosurfaceInteractor = albaInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);
  
  extractIsosurfaceInteractor->OnLeftButtonDown(dummyEventInteraction);
  
  // Left mouse button down performs a pick
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == true);

  extractIsosurfaceInteractor->Delete();
  dummyEventInteraction->Delete();
}

void albaInteractorExtractIsosurfaceTest::TestOnButtonUp()
{
  albaEventInteraction *dummyEventInteraction = new albaEventInteraction;
  dummyEventInteraction->Set2DPosition(10,20);
  double dummyPosition[2];
  dummyEventInteraction->Get2DPosition(dummyPosition);
  CPPUNIT_ASSERT(dummyPosition[0] == 10 && dummyPosition[1] == 20);

  albaInteractorExtractIsosurface *extractIsosurfaceInteractor = albaInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  extractIsosurfaceInteractor->OnButtonUp(dummyEventInteraction);
  
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);

  extractIsosurfaceInteractor->Delete();
  dummyEventInteraction->Delete();
}