/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataMirrorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFPolyDataMirrorTEST_H__
#define __CPP_UNIT_vtkMAFPolyDataMirrorTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkPolyData;

class vtkMAFPolyDataMirrorTest : public CPPUNIT_NS::TestFixture
{
	public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkMAFPolyDataMirrorTest );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestMirrorX );
  CPPUNIT_TEST( TestMirrorY );
  CPPUNIT_TEST( TestMirrorZ );
  //CPPUNIT_TEST( TestFlipNormals );
  CPPUNIT_TEST_SUITE_END();

  protected:
	void TestDynamicAllocation();
	void TestMirrorX();
  void TestMirrorY();
  void TestMirrorZ();
  void TestFlipNormals();

	vtkPolyData *m_TestPolyData;
};

#endif