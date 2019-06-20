/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeTest
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
#include "albaAttributeTest.h"

#include "albaSmartPointer.h"
#include "albaAttribute.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing Attributes. */
class albaAttributeA : public albaAttribute
//-------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaAttributeA, albaAttribute);
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaAttributeA)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaAttributeTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaAttributeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaAttributeA aa;
}
//----------------------------------------------------------------------------
void albaAttributeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaAttributeA *aa = new albaAttributeA();
  cppDEL(aa);
}
//----------------------------------------------------------------------------
void albaAttributeTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaAttributeA> aa;
  aa->SetName("test");
  albaSmartPointer<albaAttributeA> aa2;
  aa2->SetName("test");

  result = aa->Equals(aa2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaAttributeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaAttributeA> aa;
  aa->SetName("test");
  albaSmartPointer<albaAttributeA> aa2;

  aa2->DeepCopy(aa);
  
  result = aa2->Equals(aa);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaAttributeTest::TestMakeCopy()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaAttributeA> aa;
  aa->SetName("attribute a");

  result = aa->GetReferenceCount() == 1;
  TEST_RESULT;

  albaAttributeA *aa2 = (albaAttributeA *)aa->MakeCopy();
  result = aa2->Equals(aa);
  TEST_RESULT;

  result = aa2->GetReferenceCount() == 0;
  TEST_RESULT;
  cppDEL(aa2);
}
