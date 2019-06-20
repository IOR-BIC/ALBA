/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputNULLTest
 Authors: Paolo Quadrani
 
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
#include "albaVMEOutputNULLTest.h"
#include "albaVMEOutputNULL.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaVMEOutputNULLTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputNULLTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputNULL *o = albaVMEOutputNULL::New();
  albaDEL(o);
}

//----------------------------------------------------------------------------
void albaVMEOutputNULLTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  albaVMEOutputNULL *o = albaVMEOutputNULL::New();
  result = o->GetVTKData() == NULL;
  TEST_RESULT;
  albaDEL(o);
}
