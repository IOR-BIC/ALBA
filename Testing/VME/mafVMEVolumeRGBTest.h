/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeRGBTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEVolumeRGBTEST_H__
#define __CPP_UNIT_mafVMEVolumeRGBTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMEVolumeRGB.h"

class mafVMEVolumeRGBTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEVolumeRGBTest );
  CPPUNIT_TEST( TestSetDataWithImageData );
  CPPUNIT_TEST( TestSetDataWithRectilinearGrid );
  CPPUNIT_TEST( TestGetVolumeOutput );
  CPPUNIT_TEST( TestGetOutput );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestSetDataWithImageData();
  void TestSetDataWithRectilinearGrid();
  void TestGetVolumeOutput();
  void TestGetOutput();

  //help methods
  int CreateVolumeWithImageData();
  int CreateVolumeWithRectilinearGrid();

  mafVMEVolumeRGB *m_VmeVolumeRGB;
};

#endif
