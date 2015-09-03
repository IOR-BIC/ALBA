/*=========================================================================

 Program: MAF2
 Module: mafOpContextStackTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpContextStackTEST_H
#define CPP_UNIT_mafOpContextStackTEST_H

#include "mafTest.h"

class mafOpContextStackTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpContextStackTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( Test_Caller_Clear_Push_Pop );
  CPPUNIT_TEST( Test_UndoClear_UndoPush_UndoPop_UndoIsEmpty );
  CPPUNIT_TEST( Test_RedoClear_RedoPush_RedoPop_RedoIsEmpty );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void Test_Caller_Clear_Push_Pop();
    void Test_UndoClear_UndoPush_UndoPop_UndoIsEmpty();
    void Test_RedoClear_RedoPush_RedoPop_RedoIsEmpty();
};



#endif