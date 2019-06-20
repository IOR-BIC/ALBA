/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRootTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaRootTest.h"
#include "albaCoreTests.h"

#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMEStorage.h"
#include "albaEventIO.h"
#include "albaVMERoot.h"

#include <iostream>


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaRootTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaRootTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMERoot *r;
	albaNEW(r);
  albaDEL(r);
}

//----------------------------------------------------------------------------
void albaRootTest::PrintTest()
//----------------------------------------------------------------------------
{
	albaVMERoot *r;
	albaNEW(r);
	//max node id must be arbitrary 
  r->SetMaxNodeId(10);
  int id = r->GetMaxNodeId();
  std::cout << "\nTest Print:"<<std::endl;
  r->Print(std::cout, 5);
  std::cout << "End Test Print"<<std::endl;

	albaDEL(r);
}