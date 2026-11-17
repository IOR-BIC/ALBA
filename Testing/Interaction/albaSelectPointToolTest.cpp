/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectPointToolTest
 Authors: Stefano Perticoni, Gianluigi Crimi

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

#include "albaSelectPointToolTest.h"
#include "albaSelectPointTool.h"
#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"

#include <cppunit/config/SourcePrefix.h>

//----------------------------------------------------------------------------
void albaSelectPointToolTest::TestConstructorDestructor()
{
  albaSelectPointTool *tool = albaSelectPointTool::New();
  CPPUNIT_ASSERT(tool->GetCtrlModifier() == true);
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaSelectPointToolTest::TestSetGetCtrlModifier()
{
  albaSelectPointTool *tool = albaSelectPointTool::New();
  tool->SetCtrlModifierOff();
  CPPUNIT_ASSERT(tool->GetCtrlModifier() == false);
  tool->SetCtrlModifierOn();
  CPPUNIT_ASSERT(tool->GetCtrlModifier() == true);
  tool->SetCtrlModifier(false);
  CPPUNIT_ASSERT(tool->GetCtrlModifier() == false);
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaSelectPointToolTest::TestEventsWithoutRenderer()
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::New();
  albaSelectPointTool *tool = albaSelectPointTool::New();
  albaEventInteraction down(mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_LEFT_BUTTON, 1 << ALBA_CTRL_KEY);

  // no renderer: the button goes to the camera
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(&down));
  CPPUNIT_ASSERT(!tool->OnDrag(&down));

  // without CTRL the button is not used when the CTRL modifier is on
  albaEventInteraction plainDown(mouse, albaDeviceButtonsPad::GetButtonDownId(), ALBA_LEFT_BUTTON);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(&plainDown));

  tool->Delete();
  mouse->Delete();
}
