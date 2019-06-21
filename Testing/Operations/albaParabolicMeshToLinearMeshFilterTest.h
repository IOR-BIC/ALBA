/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaParabolicMeshToLinearMeshFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaParabolicMeshToLinearMeshFilterTest_H__
#define __CPP_UNIT_albaParabolicMeshToLinearMeshFilterTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "albaString.h"

class vtkFileOutputWindow;
class vtkUnstructuredGrid;

class albaParabolicMeshToLinearMeshFilterTest : public CPPUNIT_NS::TestFixture
{

public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( albaParabolicMeshToLinearMeshFilterTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestTetra4 );
  CPPUNIT_TEST( TestTetra10 );
  CPPUNIT_TEST( TestHexa8 );
  CPPUNIT_TEST( TestHexa20 );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestConstructor();
  void TestTetra4();
  void TestTetra10();
  void TestHexa8();
  void TestHexa20();

  // dataType
  enum
  {
    POINT_DATA = 0,
    CELL_DATA,
  };

  void ReadLinearizeAndDisplay(albaString &dirPrefix, int dataType = 0, bool readMaterials = false, bool writeToDisk = false);
  void RenderData( vtkUnstructuredGrid *data, int dataType = 0);

private:

  vtkFileOutputWindow               *m_Log;

};


int
  main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( albaParabolicMeshToLinearMeshFilterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
