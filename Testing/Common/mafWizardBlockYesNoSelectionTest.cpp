/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockYesNoTest
 Authors: Gianluigi Crimi
 
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

#include "mafWizardBlockYesNoSelectionTest.h"
#include "mafWizardBlockYesNoSelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockYesNoSelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{

  mafWizardBlockYesNoSelection block=mafWizardBlockYesNoSelection("testBlock");

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
void mafWizardBlockYesNoSelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlockYesNoSelection *block;

  block = new mafWizardBlockYesNoSelection("block");

  delete  block;
}