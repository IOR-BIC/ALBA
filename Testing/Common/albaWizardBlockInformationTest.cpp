/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockInformationTest
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

#include "albaWizardBlockInformationTest.h"
#include "albaWizardBlockInformation.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockInformationTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlockInformation block=albaWizardBlockInformation("test");

  block.SetWindowTitle("WindowTitle");

  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetDescription("description");

  CPPUNIT_ASSERT(block.GetDescription()=="description");

  block.SetImage("imageFilename");

  CPPUNIT_ASSERT(block.GetImage()=="imageFilename");

  block.SetBoxLabel("boxLabel");

  CPPUNIT_ASSERT(block.GetBoxLabel()=="boxLabel");
    
  block.SetImagePositionToTop();

  CPPUNIT_ASSERT(block.GetImagePositionToTop()==true);
  CPPUNIT_ASSERT(block.GetImagePositionToLeft()==false);

  block.SetImagePositionToLeft();

  CPPUNIT_ASSERT(block.GetImagePositionToLeft()==true);
  CPPUNIT_ASSERT(block.GetImagePositionToTop()==false);

}

//----------------------------------------------------------------------------
void albaWizardBlockInformationTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockInformation *block;

  block = new albaWizardBlockInformation("block");

  delete  block;
}

