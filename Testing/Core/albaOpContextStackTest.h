/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpContextStackTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpContextStackTEST_H
#define CPP_UNIT_albaOpContextStackTEST_H

#include "albaTest.h"

class albaOpContextStackTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpContextStackTest );
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