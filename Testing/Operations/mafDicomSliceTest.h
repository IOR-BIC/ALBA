/*=========================================================================

 Program: MAF2
 Module: mafDicomSliceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafDicomSliceTest_H
#define CPP_UNIT_mafDicomSliceTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafDicomSliceTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDicomSliceTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetSliceABSFileName );
  CPPUNIT_TEST( TestGetDcmInstanceNumber );
  CPPUNIT_TEST( TestGetDcmCardiacNumberOfImages );
  CPPUNIT_TEST( TestGetDcmTriggerTime );
  CPPUNIT_TEST( TestGetVTKImageData );
  CPPUNIT_TEST( TestGetDcmImagePositionPatient );
  CPPUNIT_TEST( TestGetDcmImageOrientationPatient );
  CPPUNIT_TEST( TestGetDcmImageOrientationPatientMatrix );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetSliceABSFileName();
  void TestGetDcmInstanceNumber();
  void TestGetDcmCardiacNumberOfImages();
  void TestGetDcmTriggerTime();
  void TestGetVTKImageData();
  void TestGetDcmImagePositionPatient();
  void TestGetDcmImageOrientationPatient();
  void TestGetDcmImageOrientationPatientMatrix();
};


#endif
