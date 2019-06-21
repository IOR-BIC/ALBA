/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockSelectionTest
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

#include "albaWizardBlockSelectionTest.h"
#include "albaWizardBlockSelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockSelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlockSelection block=albaWizardBlockSelection("testBlock");

  block.SetWindowTitle("WindowTitle");

  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetDescription("description");

  CPPUNIT_ASSERT(block.GetDescription()=="description");
  
}

void albaWizardBlockSelectionTest::TestAddChoice()
{

  albaWizardBlockSelection block=albaWizardBlockSelection("testBlock");

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
void albaWizardBlockSelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockSelection *block;

  block = new albaWizardBlockSelection("block");

  delete  block;
}
