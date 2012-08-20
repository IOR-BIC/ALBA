/*=========================================================================

 Program: MAF2Medical
 Module: CubeTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CubeTest_H__
#define __CPP_UNIT_CubeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class CubeTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( CubeTest );
  
  CPPUNIT_TEST( TestCornerIndex );
  CPPUNIT_TEST( FactorCornerIndexTest );

  CPPUNIT_TEST( EdgeIndexTest );
  CPPUNIT_TEST( FactorEdgeIndexTest );
  CPPUNIT_TEST( FaceIndexTest );

  CPPUNIT_TEST( FactorFaceIndexTest );
  CPPUNIT_TEST( FaceAdjacentToEdgesTest );
  CPPUNIT_TEST( EdgeCornersTest );
  CPPUNIT_TEST( FaceCornersTest );

  CPPUNIT_TEST( FaceReflectEdgeIndexTest );
  CPPUNIT_TEST( FaceReflectCornerIndexTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestCornerIndex();
  void FactorCornerIndexTest();

  void EdgeIndexTest();
  void FactorEdgeIndexTest();
  void FaceIndexTest();

  void FactorFaceIndexTest();
  void FaceAdjacentToEdgesTest();
  void EdgeCornersTest();
  void FaceCornersTest();

  void FaceReflectEdgeIndexTest();
  void FaceReflectCornerIndexTest();

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
  runner.addTest( CubeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif