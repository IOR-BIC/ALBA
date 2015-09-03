/*=========================================================================

 Program: MAF2
 Module: mafAttributeTest
 Authors: Paolo Quadrani
 
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
#include "mafAttributeTest.h"

#include "mafSmartPointer.h"
#include "mafAttribute.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing Attributes. */
class mafAttributeA : public mafAttribute
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafAttributeA, mafAttribute);
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafAttributeA)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafAttributeTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafAttributeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafAttributeA aa;
}
//----------------------------------------------------------------------------
void mafAttributeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafAttributeA *aa = new mafAttributeA();
  cppDEL(aa);
}
//----------------------------------------------------------------------------
void mafAttributeTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafAttributeA> aa;
  aa->SetName("test");
  mafSmartPointer<mafAttributeA> aa2;
  aa2->SetName("test");

  result = aa->Equals(aa2);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafAttributeTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafAttributeA> aa;
  aa->SetName("test");
  mafSmartPointer<mafAttributeA> aa2;

  aa2->DeepCopy(aa);
  
  result = aa2->Equals(aa);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafAttributeTest::TestMakeCopy()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafAttributeA> aa;
  aa->SetName("attribute a");

  result = aa->GetReferenceCount() == 1;
  TEST_RESULT;

  mafAttributeA *aa2 = (mafAttributeA *)aa->MakeCopy();
  result = aa2->Equals(aa);
  TEST_RESULT;

  result = aa2->GetReferenceCount() == 0;
  TEST_RESULT;
  cppDEL(aa2);
}
