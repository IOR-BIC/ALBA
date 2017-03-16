/*=========================================================================

 Program: MAF2
 Module: mafVMERootTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMERootTest_H__
#define __CPP_UNIT_mafVMERootTest_H__

#include "mafTest.h"

/** Test for mafVMERoot; Use this suite to trace memory problems */
class mafVMERootTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMERootTest );
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

