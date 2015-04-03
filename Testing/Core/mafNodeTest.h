/*=========================================================================

 Program: MAF2
 Module: mafNodeTest
 Authors: Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafNodeTest_H__
#define __CPP_UNIT_mafNodeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafNodeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafNodeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestSetName);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestGetAttribute);
  CPPUNIT_TEST(TestGetLink);
  CPPUNIT_TEST(TestSetLink);
  CPPUNIT_TEST(TestGetLinkSubId);
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
  CPPUNIT_TEST(TestGetNumberOfLinks);
  CPPUNIT_TEST(TestRemoveLink);
  CPPUNIT_TEST(TestRemoveAllLinks);
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
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestAddChild();
  void TestCanCopy();
  void TestCanReparentTo();
  void TestCleanTree();
  void TestCompareTree();
  void TestCopyTree();
  void TestDeepCopy();
  void TestDependsOnLinkedNode();
  void TestEquals();
  void TestFindInTreeById();
  void TestFindInTreeByName();
  void TestFindInTreeByTag();
  void TestFindNodeIdx();
  void TestGetAttribute();
  void TestGetChild();
  void TestGetChildren();
  void TestGetFirstChild();
  void TestGetLastChild();
  void TestGetLink();
  void TestGetLinkSubId();
  void TestGetNumberOfChildren();
  void TestGetNumberOfLinks();
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
  void TestRemoveAllLinks();
  void TestRemoveAttribute();
  void TestRemoveChild();
  void TestRemoveLink();
  void TestReparentTo();
  void TestSetAttribute();
  void TestSetLink();
  void TestSetName();
  void TestBuildAndDestroyATree();
  void TestGetByPath();

  bool result;
};

#endif
