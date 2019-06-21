/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockTypeCheckTest
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

#include "albaWizardBlockTypeCheckTest.h"
#include "albaWizardBlockTypeCheck.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockTypeCheckTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlockTypeCheck block=albaWizardBlockTypeCheck("testBlock");

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
void albaWizardBlockTypeCheckTest::TestAcceptList()
//----------------------------------------------------------------------------
{
  albaWizardBlockTypeCheck block=albaWizardBlockTypeCheck("block");

  block.AddAcceptedType("albaVMESurface");
  block.AddAcceptedType("albaVMEVolumeGray");
  block.AddAcceptedType("albaVMEPolylineGraph");

  std::vector < wxString > *listShow=block.GetAcceptedTypeList();

  CPPUNIT_ASSERT(listShow->size()==3);
  CPPUNIT_ASSERT((*listShow)[0]=="albaVMESurface");
  CPPUNIT_ASSERT((*listShow)[1]=="albaVMEVolumeGray");
  CPPUNIT_ASSERT((*listShow)[2]=="albaVMEPolylineGraph");

}

//----------------------------------------------------------------------------
void albaWizardBlockTypeCheckTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockTypeCheck *block;

  block = new albaWizardBlockTypeCheck("block");

  delete  block;
}
