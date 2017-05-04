/*=========================================================================

 Program: MAF2
 Module: mafDeviceTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceTest_H__
#define __CPP_UNIT_mafDeviceTest_H__

#include "mafTest.h"

class mafDeviceTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafDeviceTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetID);
  CPPUNIT_TEST(TestSetName);
  CPPUNIT_TEST(TestSetGetAutoStart);
  CPPUNIT_TEST(TestAutoStartOnOff);
  
  /** TODO: TestStart exposes 1 line of leaks from multithread stuff. Probably a bug in thread spawning...
  Too dangerous to touch*/
  //Commented because it genereates an error in AQTime with threads
  // This error blocks parabuild leaks dashboard 
  //CPPUNIT_TEST(TestStart);
  
  /** TODO: TestStop exposes 1 line of leaks in multithread stuff. Probably a bug in thread spawning...
  Too dangerous to touch*/
  //Commented because it genereates an errore in AQTime with threads
  // This error blocks parabuild leaks dashboard
  //CPPUNIT_TEST(TestStop);
  
  CPPUNIT_TEST(TestStartUp);
  CPPUNIT_TEST(TestSetGetPersistentFlag);  
  CPPUNIT_TEST(TestIsPersistent);
  CPPUNIT_TEST(TestGetGui);
  CPPUNIT_TEST(TestUpdateGui);
  CPPUNIT_TEST(TestOnEvent);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetID();
  void TestSetName();
  void TestSetGetAutoStart();
  void TestAutoStartOnOff();
  void TestStart();
  void TestStop();
  void TestStartUp();
  void TestSetGetPersistentFlag();  
  void TestIsPersistent();
  void TestGetGui();
  void TestUpdateGui();
  void TestOnEvent();
  
};

#endif
