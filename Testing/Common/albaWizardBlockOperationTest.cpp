/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockOperationTest
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

#include "albaWizardBlockOperationTest.h"
#include "albaWizardBlockOperation.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void albaWizardBlockOperationTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  albaWizardBlockOperation block=albaWizardBlockOperation("block");

  block.SetRequiredView("testView");
  CPPUNIT_ASSERT(block.GetRequiredView()=="testView");

  block.SetRequiredOperation("testOperation");
  CPPUNIT_ASSERT(block.GetRequiredOperation()=="testOperation");
  
  block.SetAutoShowSelectedVME(true);
  CPPUNIT_ASSERT(block.GetAutoShowSelectedVME()==true);

  block.SetAutoShowSelectedVME(false);
  CPPUNIT_ASSERT(block.GetAutoShowSelectedVME()==false);

  block.VmeSelect("..\firstChild");
  CPPUNIT_ASSERT(block.GetVmeSelect()=="..\firstChild");
 
}

void albaWizardBlockOperationTest::TestShowHideLists()
{

  albaWizardBlockOperation block=albaWizardBlockOperation("block");

  block.VmeShowAdd("vmeShowA");
  block.VmeShowAdd("vmeShowB");
  block.VmeShowAdd("vmeShowC");

  std::vector < wxString > *listShow=block.GetVmeShowList();

  CPPUNIT_ASSERT(listShow->size()==3);
  CPPUNIT_ASSERT((*listShow)[0]=="vmeShowA");
  CPPUNIT_ASSERT((*listShow)[1]=="vmeShowB");
  CPPUNIT_ASSERT((*listShow)[2]=="vmeShowC");

  block.VmeHideAdd("vmeHideA");
  block.VmeHideAdd("vmeHideB");
  block.VmeHideAdd("vmeHideC");

  std::vector < wxString > *listHide=block.GetVmeHideList();

  CPPUNIT_ASSERT(listHide->size()==3);
  CPPUNIT_ASSERT((*listHide)[0]=="vmeHideA");
  CPPUNIT_ASSERT((*listHide)[1]=="vmeHideB");
  CPPUNIT_ASSERT((*listHide)[2]=="vmeHideC");
}


//----------------------------------------------------------------------------
void albaWizardBlockOperationTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaWizardBlockOperation *block;

  block = new albaWizardBlockOperation("block");

  delete  block;
}

