/*=========================================================================

 Program: MAF2
 Module: mafSceneGraphTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafSceneGraphTest_H__
#define __CPP_UNIT_mafSceneGraphTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafViewVTK;
class vtkRenderer;
class mafVMERoot;

/** Test for mafSceneGraph; Use this suite to trace memory problems */
class mafSceneGraphTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafSceneGraphTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestVmeAdd);
  CPPUNIT_TEST(TestVmeRemove);
  CPPUNIT_TEST(TestVmeSelect);
  CPPUNIT_TEST(TestVme2Node);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestVmeAdd();
  void TestVmeRemove();
  void TestVmeSelect();
  void TestVme2Node();

  mafViewVTK *m_View;
  vtkRenderer *m_Ren;
  mafVMERoot *m_Vme;
  bool result;
};


#endif
