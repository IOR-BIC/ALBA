/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medWizardBlockTest.h"
#include "medWizardBlock.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void medWizardBlockTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  medWizardBlock block=medWizardBlock("testBlock");

  block.SetNextBlock("block");
  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.SetDescriptionLabel("descriptionLabel");
  CPPUNIT_ASSERT(block.GetDescriptionLabel()=="descriptionLabel");

}

//----------------------------------------------------------------------------
void medWizardBlockTest::TestConstructorDistructor()
//----------------------------------------------------------------------------
{

  medWizardBlock block=medWizardBlock("testBlock");

  CPPUNIT_ASSERT(block.GetName()=="testBlock");

}

//----------------------------------------------------------------------------
void medWizardBlockTest::TestAbort()
//----------------------------------------------------------------------------
{

  medWizardBlock block=medWizardBlock("testBlock");

  block.SetNextBlock("block");
  block.SetNextBlockOnAbort("abortBlock");

  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.Abort();

  CPPUNIT_ASSERT(block.GetNextBlock()=="abortBlock");

}

//----------------------------------------------------------------------------
void medWizardBlockTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medWizardBlock *block;

  block = new medWizardBlock("block");

  delete  block;
}
