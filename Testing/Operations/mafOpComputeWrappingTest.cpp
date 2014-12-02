/*=========================================================================

 Program: MAF2Medical
 Module: mafOpComputeWrappingTest
 Authors: Daniele Giunchi
 
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

#include "mafOpComputeWrappingTest.h"
#include "mafOpComputeWrapping.h"

#include "mafVMEVolumeGray.h"
#include "mafVMEComputeWrapping.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mafOpComputeWrappingTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpComputeWrapping *create=new mafOpComputeWrapping();
  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpComputeWrappingTest::TestOpRun() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  mafOpComputeWrapping *create=new mafOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  medVMEComputeWrapping *labeledvolume = NULL;
	labeledvolume = medVMEComputeWrapping::SafeDownCast(create->GetOutput());
  
  CPPUNIT_ASSERT(labeledvolume != NULL);

  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpComputeWrappingTest::TestCopy() 
//-----------------------------------------------------------
{
  mafOpComputeWrapping *create=new mafOpComputeWrapping();
  mafOpComputeWrapping *create2 = mafOpComputeWrapping::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpComputeWrappingTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  mafOpComputeWrapping *create=new mafOpComputeWrapping();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void mafOpComputeWrappingTest::TestOpDo() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;
  int numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 0);

  mafOpComputeWrapping *create=new mafOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  create->OpDo();
  numOfChildren = volume->GetNumberOfChildren();

  CPPUNIT_ASSERT(numOfChildren == 1);
  mafDEL(create);
}