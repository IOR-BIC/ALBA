/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafSceneGraphTest.h,v $
Language:  C++
Date:      $Date: 2008-06-24 17:19:53 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
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

int main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafSceneGraphTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
