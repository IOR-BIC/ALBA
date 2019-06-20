/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceSetTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDeviceSetTest_H__
#define __CPP_UNIT_albaDeviceSetTest_H__

#include "albaTest.h"

class albaDeviceSetTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaDeviceSetTest );
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
