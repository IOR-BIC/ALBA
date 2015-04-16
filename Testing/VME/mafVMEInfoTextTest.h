/*=========================================================================

 Program: MAF2
 Module: mafVMEInfoTextTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEInfoTextTEST_H__
#define __CPP_UNIT_mafVMEInfoTextTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

class mafVMEInfoTextTest : public CPPUNIT_NS::TestFixture
{
public :
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafVMEInfoTextTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestGetVTKOutput );
  CPPUNIT_TEST( TestGetOutput );
  CPPUNIT_TEST( TestAddString );
  CPPUNIT_TEST( TestGetSetPosShow );
  CPPUNIT_TEST( TestGetSetPosLabel );
  CPPUNIT_TEST( TestSetTimeStamp );
  CPPUNIT_TEST( TestGetIcon );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestEquals );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
	void TestGetVTKOutput();
  void TestGetOutput();
  void TestAddString();
  void TestGetSetPosShow();
  void TestGetSetPosLabel();
  void TestSetTimeStamp();
  void TestGetIcon();
  void TestDeepCopy();
  void TestEquals();

  bool result;
};

#endif
