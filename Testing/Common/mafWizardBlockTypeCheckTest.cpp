/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockTypeCheckTest
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

#include "mafWizardBlockTypeCheckTest.h"
#include "mafWizardBlockTypeCheck.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheckTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  mafWizardBlockTypeCheck block=mafWizardBlockTypeCheck("testBlock");

  block.VmeSelect("..\firstChild");
  CPPUNIT_ASSERT(block.GetVmeSelect()=="..\firstChild");

  block.SetWindowTitle("WindowTitle");
  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetDescription("description");
  CPPUNIT_ASSERT(block.GetDescription()=="description");

  block.SetWrongTypeNextBlock("abortBlock");
  CPPUNIT_ASSERT(block.GetWrongTypeNextBlock()=="abortBlock");

  block.EnableErrorMessage(false);
  CPPUNIT_ASSERT(block.IsErrorMessageEnabled()==false);

}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheckTest::TestAcceptList()
//----------------------------------------------------------------------------
{
  mafWizardBlockTypeCheck block=mafWizardBlockTypeCheck("block");

  block.AddAcceptedType("mafVMESurface");
  block.AddAcceptedType("mafVMEVolumeGray");
  block.AddAcceptedType("mafVMEPolylineGraph");

  std::vector < wxString > *listShow=block.GetAcceptedTypeList();

  CPPUNIT_ASSERT(listShow->size()==3);
  CPPUNIT_ASSERT((*listShow)[0]=="mafVMESurface");
  CPPUNIT_ASSERT((*listShow)[1]=="mafVMEVolumeGray");
  CPPUNIT_ASSERT((*listShow)[2]=="mafVMEPolylineGraph");

}

//----------------------------------------------------------------------------
void mafWizardBlockTypeCheckTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlockTypeCheck *block;

  block = new mafWizardBlockTypeCheck("block");

  delete  block;
}
