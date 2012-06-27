/*=========================================================================

 Program: MAF2
 Module: mafVMEManagerTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMETest_H__
#define __CPP_UNIT_mafVMETest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <wx/config.h>
#include <wx/fileconf.h>

class mafVMEManager;
class TestApp;

/** mafVMEManagerTest:
Test class for mafVMEManager
*/
class mafVMEManagerTest : public CPPUNIT_NS::TestFixture, public mafObserver
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEManagerTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( MSFNewTest );
  CPPUNIT_TEST( MSFOpenTest );
  CPPUNIT_TEST( ZIPOpenTest );
  CPPUNIT_TEST( MSFSaveTest );
  CPPUNIT_TEST( ZIPSaveTest );
  CPPUNIT_TEST( VmeAddRemoveTest );
  CPPUNIT_TEST( MSFIsModifiedTest );
  CPPUNIT_TEST( TimeGetBoundsTest );
  CPPUNIT_TEST( TimeSetTest );
  CPPUNIT_TEST( NotifyAddRemoveTest );
  CPPUNIT_TEST( SetGetFileNameTest );
  CPPUNIT_TEST( GetRootTest );
  CPPUNIT_TEST( GetStorageTest );
  CPPUNIT_TEST( MakeBakFileOnSaveTest );
  CPPUNIT_TEST( SetDirNameTest );
  CPPUNIT_TEST( SetHostTest );
  CPPUNIT_TEST( SetUserTest );
  CPPUNIT_TEST( SetPasswordTest ); 
  CPPUNIT_TEST( SetRemotePortTest );
  CPPUNIT_TEST( SetLocalCacheFolderTest );
  CPPUNIT_TEST( RemoveTempDirectoryTest );
  CPPUNIT_TEST( SetSingleBinaryFileTest );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void MSFNewTest();
  void MSFOpenTest();
  void ZIPOpenTest();
  void MSFSaveTest();
  void ZIPSaveTest();
  void MSFIsModifiedTest();
  void VmeAddRemoveTest();
  void TimeGetBoundsTest();
  void TimeSetTest();
  void NotifyAddRemoveTest();
  void SetGetFileNameTest();
  void GetRootTest();
  void GetStorageTest();
  void MakeBakFileOnSaveTest();
  void SetDirNameTest();
  void SetLocalCacheFolderTest();
  void SetHostTest();
  void SetUserTest();
  void SetPasswordTest();
  void SetRemotePortTest();
  void RemoveTempDirectoryTest();
  void SetSingleBinaryFileTest();

  void OnEvent(mafEventBase *maf_event); // This method trap the events coming from the vme manager

  mafVMEManager *m_Manager;
  int m_TestId;
  bool m_Result;
  wxFileConfig *m_Config;
  TestApp *m_App;
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
  runner.addTest( mafVMEManagerTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

