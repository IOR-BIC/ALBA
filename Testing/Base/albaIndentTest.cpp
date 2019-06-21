/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaIndentTest
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
#include "albaIndentTest.h"

#include "albaIndent.h"
#include "albaString.h"


#include <iostream>

//----------------------------------------------------------------------------
void albaIndentTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaIndentTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaIndent i(5);
}
//----------------------------------------------------------------------------
void albaIndentTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaIndent *i = new albaIndent(5);
  cppDEL(i);
}
//----------------------------------------------------------------------------
void albaIndentTest::TestGetTypeName()
//----------------------------------------------------------------------------
{
  albaIndent i(5);
  CPPUNIT_ASSERT(albaString(i.GetTypeName()) == albaString("albaIndent"));
}
//----------------------------------------------------------------------------
void albaIndentTest::TestGetNextIndent()
//----------------------------------------------------------------------------
{
  albaIndent i1(5);
  CPPUNIT_ASSERT(i1.GetNextIndent() == 6);
  
  albaIndent i2(20);  //ALBA_NUMBER_OF_TABS = 20 (Maximum number of supported tabs).
  CPPUNIT_ASSERT(i2.GetNextIndent() == 20); // not be 21 
}
//----------------------------------------------------------------------------
void albaIndentTest::TestOperator_int()
//----------------------------------------------------------------------------
{
  albaIndent i(5);
  CPPUNIT_ASSERT( ((int)i) == 5);
}
//----------------------------------------------------------------------------
void albaIndentTest::TestFriend_outputstream()
//----------------------------------------------------------------------------
{
  for(int number = 0; number < 10 ; number ++)
  {
    albaIndent i(number);
    std::cout << "\nBefore "<< number <<" blank space" << i << "After "<< number <<" blank space\n" << std::endl;
  }
}
