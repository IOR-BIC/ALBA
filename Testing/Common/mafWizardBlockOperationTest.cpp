/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockOperationTest
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

#include "mafWizardBlockOperationTest.h"
#include "mafWizardBlockOperation.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void mafWizardBlockOperationTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  mafWizardBlockOperation block=mafWizardBlockOperation("block");

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

void mafWizardBlockOperationTest::TestShowHideLists()
{

  mafWizardBlockOperation block=mafWizardBlockOperation("block");

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
void mafWizardBlockOperationTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafWizardBlockOperation *block;

  block = new mafWizardBlockOperation("block");

  delete  block;
}

