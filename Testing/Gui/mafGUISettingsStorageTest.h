/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsStorageTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITmafGUIStorageSettingsTest_H__
#define __CPP_UNITmafGUIStorageSettingsTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class TestApp;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafGUISettingsStorageTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafGUISettingsStorageTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
//  CPPUNIT_TEST(TestGetSingleFileStatus);
  CPPUNIT_TEST(TestUseRemoteStorage);
  CPPUNIT_TEST(TestGetCacheFolder);
  CPPUNIT_TEST(TestGetRemoteHostName);
  CPPUNIT_TEST(TestGetRemotePort);
  CPPUNIT_TEST(TestGetUserName);
  CPPUNIT_TEST(TestGetPassword);
//  CPPUNIT_TEST(TestSetSingleFileStatus);
  CPPUNIT_TEST(TestSetUseRemoteStorage);
  CPPUNIT_TEST(TestSetCacheFolder);
  CPPUNIT_TEST(TestSetRemoteHostName);
  CPPUNIT_TEST(TestSetRemotePort);
  CPPUNIT_TEST(TestSetUserName);
  CPPUNIT_TEST(TestSetPassword);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();

  // Test default parameters (Get or query methods)
//  void TestGetSingleFileStatus();
  void TestUseRemoteStorage();
  void TestGetCacheFolder();
  void TestGetRemoteHostName();
  void TestGetRemotePort();
  void TestGetUserName();
  void TestGetPassword();
  
  // Test the methods used to change the default settings.
//  void TestSetSingleFileStatus();
  void TestSetUseRemoteStorage();
  void TestSetCacheFolder();
  void TestSetRemoteHostName();
  void TestSetRemotePort();
  void TestSetUserName();
  void TestSetPassword();

  // Reset function used to clean the registry.
  void RemoveRegistryKeys();

  // helper function used to check registry with long value
  bool CheckRegistryValue(char *key, int refVal);

  // helper function used to check registry with string value
  bool CheckRegistryValue(char *key, const char *refVal);

  TestApp *m_App;
  bool result;
};

int
main( int argc, char* argv[] )
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
  runner.addTest( mafGUISettingsStorageTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
