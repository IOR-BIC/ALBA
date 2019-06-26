/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERootTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMERootTest_H__
#define __CPP_UNIT_albaVMERootTest_H__

#include "albaTest.h"

/** Test for albaVMERoot; Use this suite to trace memory problems */
class albaVMERootTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMERootTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestGetLocalTimeStamps);
  CPPUNIT_TEST(TestSetMaxItemId);
  CPPUNIT_TEST(TestResetMaxItemId);
  CPPUNIT_TEST(TestGetNextItemId);
  CPPUNIT_TEST(TestCleanTree);
  CPPUNIT_TEST(TestCanReparentTo);

	CPPUNIT_TEST(TestDynamicAllocation);
	CPPUNIT_TEST(GetMaxNodeIdTest);
	CPPUNIT_TEST(GetNextNodeIdTest);
	CPPUNIT_TEST(SetMaxNodeIdTest);
	CPPUNIT_TEST(ResetMaxNodeIdTest);
	CPPUNIT_TEST(CanReparentToTest);
	CPPUNIT_TEST(SafeDownCastTest);
	CPPUNIT_TEST(OnRootEventTest);
	CPPUNIT_TEST(GetStorageTest);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestEquals();
  void TestDeepCopy();
  void TestGetLocalTimeStamps();
  void TestSetMaxItemId();
  void TestResetMaxItemId();
  void TestGetNextItemId();
  void TestCleanTree();
  void TestCanReparentTo();

	void GetMaxNodeIdTest();
	void GetNextNodeIdTest();
	void SetMaxNodeIdTest();
	void ResetMaxNodeIdTest();
	void CanReparentToTest();
	void SafeDownCastTest();
	void OnRootEventTest();
	void GetStorageTest();
  bool result;
};


#endif

