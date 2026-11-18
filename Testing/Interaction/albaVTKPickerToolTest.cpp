/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKPickerToolTest
 Authors: Josef Kohout, Gianluigi Crimi

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

#include "albaVTKPickerToolTest.h"
#include "albaVTKPickerTool.h"

#include <cppunit/config/SourcePrefix.h>

//----------------------------------------------------------------------------
void albaVTKPickerToolTest::TestConstructorDestructor()
{
  albaVTKPickerTool *tool = albaVTKPickerTool::New();
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaVTKPickerToolTest::TestSetGetContinuousPicking()
{
  albaVTKPickerTool *tool = albaVTKPickerTool::New();
  CPPUNIT_ASSERT(tool->GetContinuousPicking() == false);
  tool->SetContinuousPicking(true);
  CPPUNIT_ASSERT(tool->GetContinuousPicking() == true);
  tool->SetContinuousPicking(false);
  CPPUNIT_ASSERT(tool->GetContinuousPicking() == false);
  tool->Delete();
}

//----------------------------------------------------------------------------
void albaVTKPickerToolTest::TestEventsWithoutRenderer()
{
  albaVTKPickerTool *tool = albaVTKPickerTool::New();
  double pos[2] = {10, 10};
  tool->SetModifierKeys(true, false, false); // CTRL down

  // no renderer: the button goes to the camera; nothing picked, nothing to send on button up
  CPPUNIT_ASSERT(!tool->OnLeftButtonDown(NULL, NULL, pos));
  CPPUNIT_ASSERT(tool->OnLeftButtonUp(NULL, NULL, pos));

  tool->Delete();
}
