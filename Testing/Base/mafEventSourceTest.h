/*=========================================================================

 Program: MAF2
 Module: mafEventSourceTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafEventSourceTest_H__
#define __CPP_UNIT_mafEventSourceTest_H__

#include "mafTest.h"

/** Test for mafEventBase; Use this suite to trace memory problems */
class mafEventSourceTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafEventSourceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAddObserver);
  CPPUNIT_TEST(TestRemoveObserver);
  CPPUNIT_TEST(TestRemoveAllObservers);
  CPPUNIT_TEST(TestIsObserver);
  CPPUNIT_TEST(TestHasObservers);
  CPPUNIT_TEST(TestInvokeEvent);
  CPPUNIT_TEST(TestSetChannel);
  CPPUNIT_TEST_SUITE_END();
  

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAddObserver();
  void TestRemoveObserver();
  void TestRemoveAllObservers();
  void TestIsObserver();
  void TestHasObservers();
  void TestInvokeEvent();
  void TestSetChannel();
};


#endif
