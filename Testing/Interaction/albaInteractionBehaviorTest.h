/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehaviorTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractionBehaviorTest_H__
#define __CPP_UNIT_albaInteractionBehaviorTest_H__

#include "albaTest.h"

class albaInteractionBehaviorTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaInteractionBehaviorTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestSetGetRenderer );
  CPPUNIT_TEST( TestButtonsNotUsed );
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetRenderer();
  void TestButtonsNotUsed();
};

#endif
