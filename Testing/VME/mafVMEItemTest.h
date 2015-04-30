/*=========================================================================

 Program: MAF2
 Module: mafVMEItemTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#endif // #ifndef __mafVMEItemTest_H__