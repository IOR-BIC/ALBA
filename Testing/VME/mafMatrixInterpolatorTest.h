/*=========================================================================

 Program: MAF2
 Module: mafMatrixInterpolatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafMatrixInterpolatorTest_H__
#define __CPP_UNIT_mafMatrixInterpolatorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** 
Class Name: mafMatrixInterpolatorTest.
Test for mafMatrixInterpolatorTest; 
Use this suite to trace memory problems 
*/

class mafMatrixInterpolatorTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test */ 
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  /** CPPUNIT test suite START*/
  CPPUNIT_TEST_SUITE( mafMatrixInterpolatorTest );
  
  /** Test to control Creation/Destruction of the object */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** Test to control if the datapipe accept correctly vme types */
  CPPUNIT_TEST(TestAccept);
  /** Test to if current item is corrected and if changing timestamp and then update are successful */
  CPPUNIT_TEST(TestGetCurrentItem_SetTimeStamp_Update);
  /** Test if modified time changes after an update*/
  CPPUNIT_TEST(TestGetMTime);

  /** CPPUNIT test suite END */
  CPPUNIT_TEST_SUITE_END();

private:
  /** Creation/Destruction of the object*/
  void TestDynamicAllocation();
  /** Call Accept function of Datapipe */
  void TestAccept();
  /** Retrieve Current item and control if data is correct, use SetTimeStamp and Update the pipe */
  void TestGetCurrentItem_SetTimeStamp_Update();
  /** Check the Modified time of the pipe*/
  void TestGetMTime();
	

  bool m_Result;
};

#endif

