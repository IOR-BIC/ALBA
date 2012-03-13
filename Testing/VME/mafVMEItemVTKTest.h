/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEItemVTKTest.h,v $
Language:  C++
Date:      $Date: 2008-06-05 13:58:47 $
Version:   $Revision: 1.1 $
Authors:   mgiacom
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEItemVTKTest_H__
#define __CPP_UNIT_mafVMEItemVTKTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEItemVTK; Use this suite to trace memory problems */
class mafVMEItemVTKTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEItemVTKTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  //ToDo: add your test here...
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST( TestIsDataPresent );
  CPPUNIT_TEST( TestGetDataFileExtension );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestShallowCopy );
  CPPUNIT_TEST( TestReadData );
  CPPUNIT_TEST( TestInternalStoreData );
  CPPUNIT_TEST( TestStoreToArchive );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  
  // ToDo: add your test methods here...
  void TestSetData();
  void TestEquals();
  void TestIsDataPresent();
  void TestGetDataFileExtension();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestReadData();
  void TestInternalStoreData();
  void TestStoreToArchive();

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
  runner.addTest( mafVMEItemVTKTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
