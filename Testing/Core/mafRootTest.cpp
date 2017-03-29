/*=========================================================================

 Program: MAF2
 Module: mafRootTest
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

#include <cppunit/config/SourcePrefix.h>
#include "mafRootTest.h"
#include "mafCoreTests.h"

#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafEventIO.h"
#include "mafVMERoot.h"

#include <iostream>


#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafRootTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafRootTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMERoot *r;
	mafNEW(r);
  mafDEL(r);
}

//----------------------------------------------------------------------------
void mafRootTest::PrintTest()
//----------------------------------------------------------------------------
{
	mafVMERoot *r;
	mafNEW(r);
	//max node id must be arbitrary 
  r->SetMaxNodeId(10);
  int id = r->GetMaxNodeId();
  std::cout << "\nTest Print:"<<std::endl;
  r->Print(std::cout, 5);
  std::cout << "End Test Print"<<std::endl;

	mafDEL(r);
}