/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar3DConeTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAvatar3DConeTest_H__
#define __CPP_UNIT_albaAvatar3DConeTest_H__

#include "albaTest.h"

class albaAvatar3DConeTest : public albaTest
{
  public:
  
    /** Executed before each test */
    void BeforeTest();

    /** Executed after each test */
    void AfterTest();

    CPPUNIT_TEST_SUITE( albaAvatar3DConeTest );
    
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestPick);

    CPPUNIT_TEST_SUITE_END();
    
  protected:
    void TestFixture();
    void TestConstructorDestructor();
    void TestPick();
};

#endif
