/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockYesNoTest
 Authors: Gianluigi Crimi
 
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

#include "albaWizardBlockYesNoSelectionTest.h"
#include "albaWizardBlockYesNoSelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockYesNoSelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{

  albaWizardBlockYesNoSelection block=albaWizardBlockYesNoSelection("testBlock");

  block.SetWindowTitle("WindowTitle");

  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");
  
  block.SetDescription("description");

  CPPUNIT_ASSERT(block.GetDescription()=="description");

  block.SetNextBlockOnYes("yesBlock");

  CPPUNIT_ASSERT(block.GetNextBlockOnYes()=="yesBlock");

  block.SetNextBlockOnNo("noBlock");

  CPPUNIT_ASSERT(block.GetNextBlockOnNo()=="noBlock");

  block.EnableCancelButton(true);

  CPPUNIT_ASSERT(block.IsCancelButtonEnabled()==true);

  block.EnableCancelButton(false);

  CPPUNIT_ASSERT(block.IsCancelButtonEnabled()==false);

}

//----------------------------------------------------------------------------
void albaWizardBlockYesNoSelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockYesNoSelection *block;

  block = new albaWizardBlockYesNoSelection("block");

  delete  block;
}