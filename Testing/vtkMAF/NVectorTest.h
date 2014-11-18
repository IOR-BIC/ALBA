/*=========================================================================

 Program: MAF2Medical
 Module: NVectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CMatrixTestM1_H__
#define __CPP_UNIT_CMatrixTestM1_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class NVectorTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( NVectorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  
  CPPUNIT_TEST( TestOperatorRoundBrackets );
  CPPUNIT_TEST( TestOperatorSquareBrackets );

  CPPUNIT_TEST( TestSetZero );
  CPPUNIT_TEST( TestDimensions );
  CPPUNIT_TEST( TestResize );

  CPPUNIT_TEST( TestOperatorAsterisk );
  CPPUNIT_TEST( TestOperatorSlash );
  CPPUNIT_TEST( TestOperatorMinus );
  CPPUNIT_TEST( TestOperatorPlus );

  CPPUNIT_TEST( TestOperatorAsteriskEqual );
  CPPUNIT_TEST( TestOperatorSlashEqual );
  CPPUNIT_TEST( TestOperatorPlusEqual );
  CPPUNIT_TEST( TestOperatorMinusEqual );

  CPPUNIT_TEST( TestAddScaled );
  CPPUNIT_TEST( TestSubtractScaled );
  CPPUNIT_TEST( TestAdd );
  CPPUNIT_TEST( TestOperatorUnaryMinus );
  CPPUNIT_TEST( TestOperatorEqual );
  CPPUNIT_TEST( TestDot );
  CPPUNIT_TEST( TestLength );

  CPPUNIT_TEST( TestNorm );
  CPPUNIT_TEST( TestNormalize );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();

  void TestOperatorRoundBrackets();
  void TestOperatorSquareBrackets();

  void TestSetZero();
  void TestDimensions();
  void TestResize();

  void TestOperatorAsterisk();
  void TestOperatorSlash();
  void TestOperatorMinus();
  void TestOperatorPlus();

  void TestOperatorAsteriskEqual();
  void TestOperatorSlashEqual();
  void TestOperatorPlusEqual();
  void TestOperatorMinusEqual();

  void TestAddScaled();
  void TestSubtractScaled();
  void TestAdd();
  void TestOperatorUnaryMinus();
  void TestOperatorEqual();
  void TestDot();
  void TestLength();

  void TestNorm();
  void TestNormalize();
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
  runner.addTest( NVectorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif