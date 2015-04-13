/*=========================================================================

 Program: MAF2
 Module: mafDeviceManagerTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceManagerTest_H__
#define __CPP_UNIT_mafDeviceManagerTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDeviceManagerTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDeviceManagerTest );
  
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestAddDevice);
  CPPUNIT_TEST(TestRemoveDevice);
  CPPUNIT_TEST(TestGetDevice);
  CPPUNIT_TEST(TestGetNumberOfDevices);
  CPPUNIT_TEST(TestRemoveAllDevices);
  CPPUNIT_TEST(TestGetDevices);
  CPPUNIT_TEST(TestGetDeviceSet);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestAddDevice();
    void TestRemoveDevice();
    void TestGetDevice();
    void TestGetNumberOfDevices();
    void TestRemoveAllDevices();
    void TestGetDevices();
    void TestGetDeviceSet();

};

#endif
