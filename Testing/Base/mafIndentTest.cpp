/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafIndentTest.cpp,v $
Language:  C++
Date:      $Date: 2007-06-14 15:03:14 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
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
#include "mafIndentTest.h"

#include "mafIndent.h"
#include "mafString.h"


#include <iostream>

//----------------------------------------------------------------------------
void mafIndentTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafIndentTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafIndentTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafIndentTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafIndent i(5);
}
//----------------------------------------------------------------------------
void mafIndentTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafIndent *i = new mafIndent(5);
  cppDEL(i);
}
//----------------------------------------------------------------------------
void mafIndentTest::TestGetTypeName()
//----------------------------------------------------------------------------
{
  mafIndent i(5);
  CPPUNIT_ASSERT(mafString(i.GetTypeName()) == mafString("mafIndent"));
}
//----------------------------------------------------------------------------
void mafIndentTest::TestGetNextIndent()
//----------------------------------------------------------------------------
{
  mafIndent i1(5);
  CPPUNIT_ASSERT(i1.GetNextIndent() == 6);
  
  mafIndent i2(20);  //MAF_NUMBER_OF_TABS = 20 (Maximum number of supported tabs).
  CPPUNIT_ASSERT(i2.GetNextIndent() == 20); // not be 21 
}
//----------------------------------------------------------------------------
void mafIndentTest::TestOperator_int()
//----------------------------------------------------------------------------
{
  mafIndent i(5);
  CPPUNIT_ASSERT( ((int)i) == 5);
}
//----------------------------------------------------------------------------
void mafIndentTest::TestFriend_outputstream()
//----------------------------------------------------------------------------
{
  for(int number = 0; number < 10 ; number ++)
  {
    mafIndent i(number);
    std::cout << "\nBefore "<< number <<" blank space" << i << "After "<< number <<" blank space\n" << std::endl;
  }
}
