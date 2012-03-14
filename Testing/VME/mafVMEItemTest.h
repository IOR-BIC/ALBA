/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemTest.h,v $
  Language:  C++
  Date:      $Date: 2009-11-30 16:18:54 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEItemTest_H__
#define __mafVMEItemTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** mafVMEItemTest:
Tets for mafVMEItem class.
*/
class mafVMEItemTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEItemTest );

  CPPUNIT_TEST( TestFixture ); // Just to test that the fixture has no leaks

  CPPUNIT_TEST( SetGetTimeStampTest );

  CPPUNIT_TEST( DeepCopyTest );

  CPPUNIT_TEST( ShallowCopyTest );

  CPPUNIT_TEST( IsDataModifiedTest );

  CPPUNIT_TEST( SetGetURLTest );

  // CPPUNIT_TEST( ReleaseOldFileOnOffTest );

  // CPPUNIT_TEST( StoreRestoreDataTest );

  CPPUNIT_TEST( SetGetIOModeTest );

  CPPUNIT_TEST( ExtractFileFromArchiveTest );

  CPPUNIT_TEST( SetGetInputMemoryTest );

  CPPUNIT_TEST( GetTagArrayTest );

  CPPUNIT_TEST( SetGetDataTypeTest );

  CPPUNIT_TEST( SetGetIdTest );

  CPPUNIT_TEST( EqualsTest );

  CPPUNIT_TEST( SetGetGlobalCompareDataFlagTest );

  // CPPUNIT_TEST( GetBoundsTets );

  // CPPUNIT_TEST( IsWaitingDataTest );

  // CPPUNIT_TEST( GetUpdateTimeTest );

  CPPUNIT_TEST( SetGetCryptingTest );

  CPPUNIT_TEST( SetGetArchiveFileNameTest );

  CPPUNIT_TEST( SetGetTempFileNameTest );

  // CPPUNIT_TEST( UpdateItemIdTest );

  CPPUNIT_TEST( PrintTest );

  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();

  void SetGetTimeStampTest();

  void DeepCopyTest();

  void ShallowCopyTest();

  // void ReleaseDataTest(); NOT IMPLEMENTED

  void IsDataModifiedTest();

  void SetGetURLTest();

  // void ReleaseOldFileOnOffTest();
  
  // void GetDataFileExtensionTest(); NOT IMPLEMENTED

  // void StoreRestoreDataTest(); NOT IMPLEMENTED

  void SetGetIOModeTest();

  void ExtractFileFromArchiveTest();

  void SetGetInputMemoryTest();

  // void ReleaseOutputMemoryTest(); NOT IMPLEMENTED

  // void GetOutputMemoryTest(); NOT IMPLEMENTED

  void GetTagArrayTest();

  void SetGetDataTypeTest();

  void SetGetIdTest();

  void EqualsTest();

  void SetGetGlobalCompareDataFlagTest();

  // void GetBoundsTets();

  // void IsDataPresentTest(); NOT IMPLEMENTED

  // void IsWaitingDataTest();
  
  // void GetUpdateTimeTest();
  
  void SetGetCryptingTest();
 
  // void UpdateBoundsTest(); NOT IMPLEMENTED

  // void UpdateDataTest(); NOT IMPLEMENTED

  // void ReadDataTest(); NOT IMPLEMENTED

  void SetGetArchiveFileNameTest();

  // void StoreToArchiveTest(); // NOT IMPLEMENTED

  void SetGetTempFileNameTest();

  // void UpdateItemIdTest();

  void PrintTest();

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
  runner.addTest( mafVMEItemTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif // #ifndef __mafVMEItemTest_H__