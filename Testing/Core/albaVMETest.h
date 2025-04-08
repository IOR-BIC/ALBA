/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMETest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMETest_H__
#define __CPP_UNIT_albaVMETest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMETest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMETest );
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
  CPPUNIT_TEST(TestSetVisualMode);

	CPPUNIT_TEST(TestSetName);
	CPPUNIT_TEST(TestEquals);
	CPPUNIT_TEST(TestGetAttribute);
	CPPUNIT_TEST(TestDependsOnLinkedNode);
	CPPUNIT_TEST(TestCanCopy);
	CPPUNIT_TEST(TestDeepCopy);
	CPPUNIT_TEST(TestMakeCopy);
	CPPUNIT_TEST(TestReparentTo);
	CPPUNIT_TEST(TestGetParent);
	CPPUNIT_TEST(TestGetRoot);
	CPPUNIT_TEST(TestGetTagArray);
	CPPUNIT_TEST(TestIsInTree);
	CPPUNIT_TEST(TestIsAChild);
	CPPUNIT_TEST(TestIsValid);
	CPPUNIT_TEST(TestIsVisible);
	CPPUNIT_TEST(TestGetNumberOfChildren);
	CPPUNIT_TEST(TestIsEmpty);
	CPPUNIT_TEST(TestSetAttribute);
	CPPUNIT_TEST(TestRemoveAttribute);
	CPPUNIT_TEST(TestRemoveAllAttributes);
	CPPUNIT_TEST(TestCleanTree);
	CPPUNIT_TEST(TestNewIterator);
	CPPUNIT_TEST(TestCanReparentTo);
	CPPUNIT_TEST(TestGetChildren);
	CPPUNIT_TEST(TestGetFirstChild);
	CPPUNIT_TEST(TestGetLastChild);
	CPPUNIT_TEST(TestAddChild);
	CPPUNIT_TEST(TestGetChild);
	CPPUNIT_TEST(TestCompareTree);
	CPPUNIT_TEST(TestCopyTree);
	CPPUNIT_TEST(TestFindInTreeById);
	CPPUNIT_TEST(TestFindInTreeByName);
	CPPUNIT_TEST(TestFindInTreeByTag);
	CPPUNIT_TEST(TestFindNodeIdx);
	CPPUNIT_TEST(TestRemoveChild);
	CPPUNIT_TEST(TestRemoveAllChildren);
	CPPUNIT_TEST(TestImport);
	CPPUNIT_TEST(TestBuildAndDestroyATree);
	CPPUNIT_TEST(TestGetByPath);

	CPPUNIT_TEST(TestGetLink);
	CPPUNIT_TEST(TestSetLink);
	CPPUNIT_TEST(TestMandatoryLink);
	CPPUNIT_TEST(TestGetNumberOfLinks);
	CPPUNIT_TEST(TestRemoveLink);
	CPPUNIT_TEST(TestRemoveAllLinks);
	CPPUNIT_TEST(TestDependencies);

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
	void TestAddChild();
	void TestCanCopy();
	void TestCleanTree();
	void TestCompareTree();
	void TestCopyTree();
	void TestDependsOnLinkedNode();
	void TestFindInTreeById();
	void TestFindInTreeByName();
	void TestFindInTreeByTag();
	void TestFindNodeIdx();
	void TestGetAttribute();
	void TestGetChild();
	void TestGetChildren();
	void TestGetFirstChild();
	void TestGetLastChild();
	void TestGetNumberOfChildren();
	void TestGetParent();
	void TestGetRoot();
	void TestGetTagArray();
	void TestImport();
	void TestIsAChild();
	void TestIsEmpty();
	void TestIsInTree();
	void TestIsValid();
	void TestIsVisible();
	void TestMakeCopy();
	void TestNewIterator();
	void TestRemoveAllAttributes();
	void TestRemoveAllChildren();
	void TestRemoveAttribute();
	void TestRemoveChild();
	void TestReparentTo();
	void TestSetAttribute();
	void TestSetName();
	void TestBuildAndDestroyATree();
	void TestGetByPath();

	void TestSetLink();
	void TestGetLink();
	void TestMandatoryLink();
	void TestDependencies();
	void TestGetNumberOfLinks();
	void TestRemoveLink();
	void TestRemoveAllLinks();
  bool result;
};


#endif

