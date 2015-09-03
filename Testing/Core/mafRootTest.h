/*=========================================================================

 Program: MAF2
 Module: mafRootTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafRootTest_H__
#define __CPP_UNIT_mafRootTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafRootTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafRootTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(GetMaxNodeIdTest);
  CPPUNIT_TEST(GetNextNodeIdTest);
  CPPUNIT_TEST(SetMaxNodeIdTest);
  CPPUNIT_TEST(ResetMaxNodeIdTest);
  CPPUNIT_TEST(CanReparentToTest);
  CPPUNIT_TEST(SafeDownCastTest);
  CPPUNIT_TEST(OnRootEventTest);
  CPPUNIT_TEST(GetStorageTest);
  CPPUNIT_TEST(PrintTest);  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void GetMaxNodeIdTest();
  void GetNextNodeIdTest();
  void SetMaxNodeIdTest();
  void ResetMaxNodeIdTest();
  void CanReparentToTest();
  void SafeDownCastTest();
  void OnRootEventTest();
  void GetStorageTest();
  void PrintTest();

  bool result;
};


#endif
