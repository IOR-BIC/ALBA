/*=========================================================================

 Program: MAF2
 Module: mafInteractorExtractIsosurfaceTest
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

#include "mafInteractorExtractIsosurfaceTest.h"
#include "mafInteractorExtractIsosurface.h"
#include "mafDeviceButtonsPadMouse.h" 
#include "mafEventInteraction.h"

void mafInteractorExtractIsosurfaceTest::setUp()
{
 
}

void mafInteractorExtractIsosurfaceTest::tearDown()
{

}

void mafInteractorExtractIsosurfaceTest::TestFixture()
{

}

void mafInteractorExtractIsosurfaceTest::TestConstructor()
{
  mafInteractorExtractIsosurface *extractIsosurfaceInteractor = mafInteractorExtractIsosurface::New();
 
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_Renderer == NULL);

  extractIsosurfaceInteractor->Delete();
}

void mafInteractorExtractIsosurfaceTest::TestStartInteraction()
{
  mafDeviceButtonsPadMouse *buttonsPadMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(buttonsPadMouse);

  mafInteractorExtractIsosurface *extractIsosurfaceInteractor = mafInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  int returnValue = extractIsosurfaceInteractor->StartInteraction(buttonsPadMouse);
  
  // since no renderer exists this should return false
  CPPUNIT_ASSERT(returnValue == false);

  extractIsosurfaceInteractor->Delete();
  buttonsPadMouse->Delete();
}

void mafInteractorExtractIsosurfaceTest::TestOnMouseMove()
{
  mafInteractorExtractIsosurface *extractIsosurfaceInteractor = mafInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  extractIsosurfaceInteractor->OnMouseMove();

  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);

  extractIsosurfaceInteractor->Delete();
}

void mafInteractorExtractIsosurfaceTest::TestOnLeftButtonDown()
{
  mafEventInteraction *dummyEventInteraction = new mafEventInteraction;
  dummyEventInteraction->Set2DPosition(10,20);
  double dummyPosition[2];
  dummyEventInteraction->Get2DPosition(dummyPosition);
  CPPUNIT_ASSERT(dummyPosition[0] == 10 && dummyPosition[1] == 20);

  mafInteractorExtractIsosurface *extractIsosurfaceInteractor = mafInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);
  
  extractIsosurfaceInteractor->OnLeftButtonDown(dummyEventInteraction);
  
  // Left mouse button down performs a pick
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == true);

  extractIsosurfaceInteractor->Delete();
  dummyEventInteraction->Delete();
}

void mafInteractorExtractIsosurfaceTest::TestOnButtonUp()
{
  mafEventInteraction *dummyEventInteraction = new mafEventInteraction;
  dummyEventInteraction->Set2DPosition(10,20);
  double dummyPosition[2];
  dummyEventInteraction->Get2DPosition(dummyPosition);
  CPPUNIT_ASSERT(dummyPosition[0] == 10 && dummyPosition[1] == 20);

  mafInteractorExtractIsosurface *extractIsosurfaceInteractor = mafInteractorExtractIsosurface::New();
  CPPUNIT_ASSERT(extractIsosurfaceInteractor);

  extractIsosurfaceInteractor->OnButtonUp(dummyEventInteraction);
  
  CPPUNIT_ASSERT(extractIsosurfaceInteractor->m_PickValue == false);

  extractIsosurfaceInteractor->Delete();
  dummyEventInteraction->Delete();
}