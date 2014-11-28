/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockTest
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

#include "mafWizardBlockTest.h"
#include "mafWizardBlock.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  mafWizardBlock block=mafWizardBlock("testBlock");

  block.SetNextBlock("block");
  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.SetDescriptionLabel("descriptionLabel");
  CPPUNIT_ASSERT(block.GetDescriptionLabel()=="descriptionLabel");

  block.SetBlockProgress(45);
  CPPUNIT_ASSERT(block.GetBlockProgress()==45);

}

//----------------------------------------------------------------------------
void mafWizardBlockTest::TestConstructorDistructor()
//----------------------------------------------------------------------------
{

  mafWizardBlock block=mafWizardBlock("testBlock");

  CPPUNIT_ASSERT(block.GetName()=="testBlock");

}

//----------------------------------------------------------------------------
void mafWizardBlockTest::TestAbort()
//----------------------------------------------------------------------------
{

  mafWizardBlock block=mafWizardBlock("testBlock");

  block.SetNextBlock("block");
  block.SetNextBlockOnAbort("abortBlock");

  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.Abort();

  CPPUNIT_ASSERT(block.GetNextBlock()=="abortBlock");

}

//----------------------------------------------------------------------------
void mafWizardBlockTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlock *block;

  block = new mafWizardBlock("block");

  delete  block;
}
