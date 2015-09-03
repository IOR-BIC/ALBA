/*=========================================================================

 Program: MAF2
 Module: mafDeviceSetTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceSetTest_H__
#define __CPP_UNIT_mafDeviceSetTest_H__

#include "mafTest.h"

class mafDeviceSetTest : public mafTest
{
  public:
  
  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafDeviceSetTest );
  
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestGetDeviceByIndex);
  CPPUNIT_TEST(TestGetDeviceByID);
  CPPUNIT_TEST(TestGetDeviceByName);
  CPPUNIT_TEST(TestGetNumberOfDevices);
  CPPUNIT_TEST(TestRemoveDeviceByIndex);
  CPPUNIT_TEST(TestRemoveDeviceByID);
  CPPUNIT_TEST(TestRemoveDeviceByName);
  CPPUNIT_TEST(TestRemoveDeviceByPointer);
  CPPUNIT_TEST(TestRemoveAllDevices);
  CPPUNIT_TEST(TestGetDevices);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();
    void TestGetDeviceByIndex();
	  void TestGetDeviceByID();
    void TestGetDeviceByName();
    void TestGetNumberOfDevices();
    void TestRemoveDeviceByIndex();
    void TestRemoveDeviceByID();
    void TestRemoveDeviceByName();
    void TestRemoveDeviceByPointer();
    void TestRemoveAllDevices();
    void TestGetDevices();


};

#endif
