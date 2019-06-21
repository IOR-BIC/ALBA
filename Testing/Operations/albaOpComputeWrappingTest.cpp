/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrappingTest
 Authors: Daniele Giunchi
 
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

#include "albaOpComputeWrappingTest.h"
#include "albaOpComputeWrapping.h"

#include "albaVMEVolumeGray.h"
#include "albaVMEComputeWrapping.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void albaOpComputeWrappingTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpComputeWrapping *create=new albaOpComputeWrapping();
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeWrappingTest::TestOpRun() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;

  albaOpComputeWrapping *create=new albaOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  albaVMEComputeWrapping *labeledvolume = NULL;
	labeledvolume = albaVMEComputeWrapping::SafeDownCast(create->GetOutput());
  
  CPPUNIT_ASSERT(labeledvolume != NULL);

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeWrappingTest::TestCopy() 
//-----------------------------------------------------------
{
  albaOpComputeWrapping *create=new albaOpComputeWrapping();
  albaOpComputeWrapping *create2 = albaOpComputeWrapping::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  albaDEL(create2);
  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeWrappingTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;

  albaOpComputeWrapping *create=new albaOpComputeWrapping();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  albaDEL(create);
}
//-----------------------------------------------------------
void albaOpComputeWrappingTest::TestOpDo() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;
  int numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 0);

  albaOpComputeWrapping *create=new albaOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  create->OpDo();
  numOfChildren = volume->GetNumberOfChildren();

  CPPUNIT_ASSERT(numOfChildren == 1);
  albaDEL(create);
}