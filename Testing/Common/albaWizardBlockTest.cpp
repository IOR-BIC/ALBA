/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockTest
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

#include "albaWizardBlockTest.h"
#include "albaWizardBlock.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlock block=albaWizardBlock("testBlock");

  block.SetNextBlock("block");
  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.SetDescriptionLabel("descriptionLabel");
  CPPUNIT_ASSERT(block.GetDescriptionLabel()=="descriptionLabel");

  block.SetBlockProgress(45);
  CPPUNIT_ASSERT(block.GetBlockProgress()==45);

}

//----------------------------------------------------------------------------
void albaWizardBlockTest::TestConstructorDistructor()
//----------------------------------------------------------------------------
{

  albaWizardBlock block=albaWizardBlock("testBlock");

  CPPUNIT_ASSERT(block.GetName()=="testBlock");

}

//----------------------------------------------------------------------------
void albaWizardBlockTest::TestAbort()
//----------------------------------------------------------------------------
{

  albaWizardBlock block=albaWizardBlock("testBlock");

  block.SetNextBlock("block");
  block.SetNextBlockOnAbort("abortBlock");

  CPPUNIT_ASSERT(block.GetNextBlock()=="block");

  block.Abort();

  CPPUNIT_ASSERT(block.GetNextBlock()=="abortBlock");

}

//----------------------------------------------------------------------------
void albaWizardBlockTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlock *block;

  block = new albaWizardBlock("block");

  delete  block;
}
