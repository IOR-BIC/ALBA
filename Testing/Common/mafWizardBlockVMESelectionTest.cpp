/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockVMESelectionTest
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

#include "mafWizardBlockVMESelectionTest.h"
#include "mafWizardBlockVMESelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockVMESelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  mafWizardBlockVMESelection block=mafWizardBlockVMESelection("testBlock");

  block.SetWindowTitle("WindowTitle");
  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetAcceptedVME("mafVMESurface");
  CPPUNIT_ASSERT(block.GetAcceptedVME()=="mafVMESurface");
  
}


//----------------------------------------------------------------------------
void mafWizardBlockVMESelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlockVMESelection *block;

  block = new mafWizardBlockVMESelection("block");

  delete  block;
}
