/*=========================================================================

 Program: MAF2
 Module: mafDicomCardiacMRIHelperTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafDicomCardiacMRIHelperTest_H
#define CPP_UNIT_mafDicomCardiacMRIHelperTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

using namespace std;
 
class mafDicomCardiacMRIHelperTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDicomCardiacMRIHelperTest );

    CPPUNIT_TEST(TestConstructorDestructor);
    CPPUNIT_TEST(TestSetGetInputDicomDirectoryABSPath);

  /**
  
  For the moment the following two test cases must be validated manually 
  against filemapp.m output (copy/paste filemapp.m matlab code from mafDicomCardiacMRIHelper.h) 
  Execute matlab code and compare with test outputs
  */
  //  CPPUNIT_TEST(TestDicomGeneralElectricPisa);
  //  CPPUNIT_TEST(TestDicomSiemensNiguarda);
  
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructorDestructor();

  void TestSetGetInputDicomDirectoryABSPath();

  /**
  Validate this code against filemapp.m matlab code from mafDicomCardiacMRIHelper.h */
  void TestDicomGeneralElectricPisa();
  
   /**
  Validate this code against filemapp.m matlab code from mafDicomCardiacMRIHelper.h */
  void TestDicomSiemensNiguarda();
};

#endif
