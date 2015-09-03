/*=========================================================================

 Program: MAF2
 Module: mafVMETest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMETest_H__
#define __CPP_UNIT_mafVMETest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMETest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMETest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestCanReparentTo);

  CPPUNIT_TEST(TestSetTimeStamp);

  CPPUNIT_TEST(TestSetAbsMatrix);
  CPPUNIT_TEST(TestSetAbsPose);
  CPPUNIT_TEST(TestApplyAbsMatrix);

  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestShallowCopy);
  CPPUNIT_TEST(TestIsAnimated);
  CPPUNIT_TEST(TestIsDataAvailable);

  CPPUNIT_TEST(TestSetBehavior);
  CPPUNIT_TEST(TestSetCrypting);

  CPPUNIT_TEST(TestSetParent);

  CPPUNIT_TEST(TestSetMatrix);
  CPPUNIT_TEST(TestSetPose);
  CPPUNIT_TEST(TestApplyMatrix);
  
  CPPUNIT_TEST(TestSetTreeTime);
  //CPPUNIT_TEST(TestSetVisualMode);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestCanReparentTo();
  void TestEquals();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestIsAnimated();
  void TestIsDataAvailable();

  void TestSetAbsMatrix();
  void TestSetAbsPose();
  void TestApplyAbsMatrix();

  void TestSetBehavior();
  void TestSetCrypting();

  void TestSetParent();

  void TestSetMatrix();
  void TestSetPose();
  void TestApplyMatrix();

  void TestSetTimeStamp();
  void TestSetTreeTime();

  void TestSetVisualMode();

  bool result;
};


#endif

