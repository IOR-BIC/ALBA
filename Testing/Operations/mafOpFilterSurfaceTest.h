/*=========================================================================

 Program: MAF2
 Module: mafOpFilterSurfaceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpFilterSurfaceTest_H__
#define __CPP_UNIT_mafOpFilterSurfaceTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpFilterSurfaceTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpFilterSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOnClean );
  CPPUNIT_TEST( TestOnSmooth );
  CPPUNIT_TEST( TestOnDecimate );
  CPPUNIT_TEST( TestOnTriangulate );
  CPPUNIT_TEST( TestOnVtkConnect );
  CPPUNIT_TEST( TestOnStripper );
  CPPUNIT_TEST( TestOnGenerateNormals );
  CPPUNIT_TEST( TestUndo );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnClean();
  void TestOnSmooth();
  void TestOnDecimate();
  void TestOnTriangulate();
  void TestOnVtkConnect();
  void TestOnStripper();
  void TestOnGenerateNormals();
  void TestUndo();

  void CheckPolydata(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  void CheckNormals(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  bool m_Result;
};

#endif
