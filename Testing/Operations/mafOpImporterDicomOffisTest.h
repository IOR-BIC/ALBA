/*=========================================================================

 Program: MAF2
 Module: mafOpImporterDicomOffisTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterDicomOffisTest_H
#define CPP_UNIT_mafOpImporterDicomOffisTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
using namespace std;
 
class mafOpImporterDicomOffisTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafOpImporterDicomOffisTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestAccept );
    CPPUNIT_TEST( TestSetDirName );
    CPPUNIT_TEST( TestCreateVolume );
    CPPUNIT_TEST( TestCompareDicomImage );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
  void TestSetDirName();
  void TestCompareDicomImage();
  void TestCreateVolume();

};

#endif
