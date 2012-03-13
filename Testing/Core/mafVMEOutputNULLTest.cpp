/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputNULLTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-24 17:29:30 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEOutputNULLTest.h"
#include "mafVMEOutputNULL.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafVMEOutputNULLTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputNULLTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputNULLTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputNULLTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputNULL *o = mafVMEOutputNULL::New();
  mafDEL(o);
}

//----------------------------------------------------------------------------
void mafVMEOutputNULLTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  mafVMEOutputNULL *o = mafVMEOutputNULL::New();
  result = o->GetVTKData() == NULL;
  TEST_RESULT;
  mafDEL(o);
}
