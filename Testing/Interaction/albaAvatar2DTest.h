/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar2DTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAvatar2DTest_H__
#define __CPP_UNIT_albaAvatar2DTest_H__

#include "albaTest.h"

class albaAvatar2DTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( albaAvatar2DTest );
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestOnEvent);
    CPPUNIT_TEST(TestSetMouse);
    CPPUNIT_TEST(TestGetMouse);
    CPPUNIT_TEST_SUITE_END();
    
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestOnEvent();
    void TestSetMouse();
    void TestGetMouse();

};

#endif
