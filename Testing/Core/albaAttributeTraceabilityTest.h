/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeTraceabilityTest
 Authors: Roberto Mucci, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAttributeTraceability_H__
#define __CPP_UNIT_albaAttributeTraceability_H__

#include "albaTest.h"

/** Test for albaAttributeTraceability; Use this suite to trace memory problems */
class albaAttributeTraceabilityTest : public albaTest
{
public: 

	// CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaAttributeTraceabilityTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestAddTraceabilityEvent);
  CPPUNIT_TEST(TestRemoveTraceabilityEvent);
  CPPUNIT_TEST(TestGetLastTrialEvent);
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestEquals);
	CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAddTraceabilityEvent();
  void TestRemoveTraceabilityEvent();
  void TestGetLastTrialEvent();
  void TestDeepCopy();
  void TestEquals();


  bool result;
};


#endif
