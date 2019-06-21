/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadTest
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
#include "albaDeviceButtonsPadTest.h"
#include "albaDeviceButtonsPad.h"


void albaDeviceButtonsPadTest::TestConstructorDestructor()
{
 albaDeviceButtonsPad *bp = albaDeviceButtonsPad::New();
 bp->Delete();
}

void albaDeviceButtonsPadTest::TestSetGetNumberOfButtons()
{
 albaDeviceButtonsPad *buttonsPad = albaDeviceButtonsPad::New();
 int defaultButtonsNumber = buttonsPad->GetNumberOfButtons();
 CPPUNIT_ASSERT_EQUAL(defaultButtonsNumber, 3);

 buttonsPad->SetNumberOfButtons(2);
 CPPUNIT_ASSERT_EQUAL(buttonsPad->GetNumberOfButtons(), 2);
 buttonsPad->Delete();
}

void albaDeviceButtonsPadTest::TestSetGetButtonState()
{
 albaDeviceButtonsPad *buttonsPad = albaDeviceButtonsPad::New();

 for (int i = 0; i < buttonsPad->GetNumberOfButtons(); i++) 
 {
   bool defaultButtonState = buttonsPad->GetButtonState(i);
   CPPUNIT_ASSERT_EQUAL(defaultButtonState, false);

   buttonsPad->SetButtonState(i, true);
   CPPUNIT_ASSERT_EQUAL(buttonsPad->GetButtonState(i), true);
 }

 buttonsPad->Delete();
}

void albaDeviceButtonsPadTest::TestFixture()
{
}
