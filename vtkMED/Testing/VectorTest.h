/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: VectorTest.h,v $
Language:  C++
Date:      $Date: 2011-01-10 12:06:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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


class VectorTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( VectorTest );
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
  runner.addTest( VectorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif