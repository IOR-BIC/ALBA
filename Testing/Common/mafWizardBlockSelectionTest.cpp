/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockSelectionTest
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

#include "mafWizardBlockSelectionTest.h"
#include "mafWizardBlockSelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockSelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  mafWizardBlockSelection block=mafWizardBlockSelection("testBlock");

  block.SetWindowTitle("WindowTitle");

  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetDescription("description");

  CPPUNIT_ASSERT(block.GetDescription()=="description");
  
}

void mafWizardBlockSelectionTest::TestAddChoice()
{

  mafWizardBlockSelection block=mafWizardBlockSelection("testBlock");

  block.AddChoice("choice1","block1");
  block.AddChoice("choice2","block2");
  block.AddChoice("choice3","block3");

  CPPUNIT_ASSERT(block.GetChoiceLabel(0)=="choice1");
  CPPUNIT_ASSERT(block.GetChoiceNextBlock(0)=="block1");

  CPPUNIT_ASSERT(block.GetChoiceLabel(1)=="choice2");
  CPPUNIT_ASSERT(block.GetChoiceNextBlock(1)=="block2");

  CPPUNIT_ASSERT(block.GetChoiceLabel(2)=="choice3");
  CPPUNIT_ASSERT(block.GetChoiceNextBlock(2)=="block3");

  //out of bounds tests
  CPPUNIT_ASSERT(block.GetChoiceLabel(-1)=="");
  CPPUNIT_ASSERT(block.GetChoiceNextBlock(3)=="");

}


//----------------------------------------------------------------------------
void mafWizardBlockSelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlockSelection *block;

  block = new mafWizardBlockSelection("block");

  delete  block;
}
