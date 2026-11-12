/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaExtractIsosurfaceToolTest
 Authors: Paolo Quadrani, Gianluigi Crimi

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

#include "albaExtractIsosurfaceToolTest.h"
#include "albaExtractIsosurfaceTool.h"
#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"

#include <cppunit/config/SourcePrefix.h>

//----------------------------------------------------------------------------
void albaExtractIsosurfaceToolTest::TestConstructorDestructor()
{
  albaExtractIsosurfaceTool *tool = albaExtractIsosurfaceTool::New();
  CPPUNIT_ASSERT(tool->m_PickValue == false);
  CPPUNIT_ASSERT(tool->m_Renderer == NULL);
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaExtractIsosurfaceToolTest::TestEventsWithoutRenderer()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaExtractIsosurfaceTool *tool = albaExtractIsosurfaceTool::New();
  albaEventInteraction down(mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_LEFT_BUTTON);

  // no renderer: the button goes to the camera
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(&down));
  CPPUNIT_ASSERT(tool->m_PickValue == false);

  // a drag cancels the picking and moves the camera
  tool->m_PickValue = true;
  CPPUNIT_ASSERT(!tool->OnDrag(&down));
  CPPUNIT_ASSERT(tool->m_PickValue == false);
  CPPUNIT_ASSERT(tool->OnLeftButtonUp(&down));

  tool->Delete();
  mouse->Delete();
}
