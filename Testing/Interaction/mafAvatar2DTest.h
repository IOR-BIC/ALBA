/*=========================================================================

 Program: MAF2
 Module: mafAvatar2DTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAvatar2DTest_H__
#define __CPP_UNIT_mafAvatar2DTest_H__

#include "mafTest.h"

class mafAvatar2DTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( mafAvatar2DTest );
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
