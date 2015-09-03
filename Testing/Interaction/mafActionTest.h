/*=========================================================================

 Program: MAF2
 Module: mafActionTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafActionTest_H__
#define __CPP_UNIT_mafActionTest_H__

#include "mafTest.h"

class mafActionTest : public mafTest
{
  public:
  
 
    CPPUNIT_TEST_SUITE( mafActionTest );
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestSetGetType);
    CPPUNIT_TEST(TestSetTypeToShared);
    CPPUNIT_TEST(TestSetTypeToExclusive);
    CPPUNIT_TEST(TestBindDevice);
    CPPUNIT_TEST(TestUnBindDevice);
    CPPUNIT_TEST(TestBindInteractor);
    CPPUNIT_TEST(TestUnBindInteractor);
    CPPUNIT_TEST(TestGetDevices);
    CPPUNIT_TEST_SUITE_END();
    
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestSetGetType();
    void TestSetTypeToShared();
    void TestSetTypeToExclusive();
    void TestBindDevice();
    void TestUnBindDevice();
    void TestBindInteractor();
    void TestUnBindInteractor();
    void TestGetDevices();
};

#endif
