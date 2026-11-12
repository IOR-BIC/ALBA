/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCompositorMouseBehaviorTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaCompositorMouseBehaviorTest_H__
#define __CPP_UNIT_albaCompositorMouseBehaviorTest_H__

#include "albaTest.h"

class albaCompositorMouseBehaviorTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaCompositorMouseBehaviorTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestStartInteraction);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestCreateBehavior);
  CPPUNIT_TEST(TestGetBehavior);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestStartInteraction();
    void TestOnEvent();
    void TestCreateBehavior();
    void TestGetBehavior();    
};

#endif
