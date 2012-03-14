/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceButtonsPadTest.cpp,v $
Language:  C++
Date:      $Date: 2009-05-25 14:51:51 $
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
#include "mafDeviceButtonsPadTest.h"
#include "mafDeviceButtonsPad.h"

void mafDeviceButtonsPadTest::setUp()
{

}

void mafDeviceButtonsPadTest::tearDown()
{

}


void mafDeviceButtonsPadTest::TestConstructorDestructor()
{
 mafDeviceButtonsPad *bp = mafDeviceButtonsPad::New();
 bp->Delete();
}

void mafDeviceButtonsPadTest::TestSetGetNumberOfButtons()
{
 mafDeviceButtonsPad *buttonsPad = mafDeviceButtonsPad::New();
 int defaultButtonsNumber = buttonsPad->GetNumberOfButtons();
 CPPUNIT_ASSERT_EQUAL(defaultButtonsNumber, 3);

 buttonsPad->SetNumberOfButtons(2);
 CPPUNIT_ASSERT_EQUAL(buttonsPad->GetNumberOfButtons(), 2);
 buttonsPad->Delete();
}

void mafDeviceButtonsPadTest::TestSetGetButtonState()
{
 mafDeviceButtonsPad *buttonsPad = mafDeviceButtonsPad::New();

 for (int i = 0; i < buttonsPad->GetNumberOfButtons(); i++) 
 {
   bool defaultButtonState = buttonsPad->GetButtonState(i);
   CPPUNIT_ASSERT_EQUAL(defaultButtonState, false);

   buttonsPad->SetButtonState(i, true);
   CPPUNIT_ASSERT_EQUAL(buttonsPad->GetButtonState(i), true);
 }

 buttonsPad->Delete();
}

void mafDeviceButtonsPadTest::TestFixture()
{
}
