/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputVolumeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputVolumeTest_H__
#define __CPP_UNIT_mafVMEOutputVolumeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputVolumeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetStructuredData );
  CPPUNIT_TEST( TestGetRectilinearData );
  CPPUNIT_TEST( TestGetUnstructuredData );
  CPPUNIT_TEST( TestSetGetVolumeMaterial );
  CPPUNIT_TEST( TestGetVTKDataTypeAsString_Update );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetStructuredData();
  void TestGetRectilinearData();
  void TestGetUnstructuredData();
  void TestSetGetVolumeMaterial();
  void TestGetVTKDataTypeAsString_Update();

	bool m_Result;
};

#endif
