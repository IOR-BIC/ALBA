/*=========================================================================

 Program: MAF2
 Module: mafVMEVolumeLargeUtilsTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEVolumeLargeUtilsTest_H__
#define __CPP_UNIT_mafVMEVolumeLargeUtilsTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafString.h"

class mafVMEVolumeLargeUtilsTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEVolumeLargeUtilsTest );

  CPPUNIT_TEST(GetFileLengthTest);
  CPPUNIT_TEST(IsFileLargeTest);
  CPPUNIT_TEST(IsVolumeLargeTest);

  CPPUNIT_TEST_SUITE_END();

protected:

  void GetFileLengthTest();
  void IsFileLargeTest();
  void IsVolumeLargeTest();

  mafString m_TestFilePath;
};

#endif