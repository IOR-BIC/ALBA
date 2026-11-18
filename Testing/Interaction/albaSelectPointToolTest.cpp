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
  albaSelectPointTool *tool = albaSelectPointTool::New();
  double pos[2] = {10, 10};
  tool->SetModifierKeys(true, false, false); // CTRL down

  // no renderer: the button goes to the camera
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(!tool->OnDrag(NULL, NULL, pos));

  // without CTRL the button is not used when the CTRL modifier is on
  tool->SetModifierKeys(false, false, false);
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));

  tool->Delete();
}
