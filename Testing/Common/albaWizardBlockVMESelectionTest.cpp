/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockVMESelectionTest
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

#include "albaWizardBlockVMESelectionTest.h"
#include "albaWizardBlockVMESelection.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockVMESelectionTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlockVMESelection block=albaWizardBlockVMESelection("testBlock");

  block.SetWindowTitle("WindowTitle");
  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetAcceptedVME("albaVMESurface");
  CPPUNIT_ASSERT(block.GetAcceptedVME()=="albaVMESurface");
  
}


//----------------------------------------------------------------------------
void albaWizardBlockVMESelectionTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockVMESelection *block;

  block = new albaWizardBlockVMESelection("block");

  delete  block;
}
