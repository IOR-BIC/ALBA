/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpComputeInertialTensorTest.h,v $
Language:  C++
Date:      $Date: 2011-02-14 11:37:18 $
Version:   $Revision: 1.1.2.1 $
Authors:   Simone Brazzale , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_medOpComputeInertialTensorTest_H
#define CPP_UNIT_medOpComputeInertialTensorTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpComputeInertialTensorTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( medOpComputeInertialTensorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestAddAttributes );
  CPPUNIT_TEST( TestComputeInertialTensorFromDefaultValue );
  CPPUNIT_TEST( TestComputeInertialTensorFromDENSITYTag );
  CPPUNIT_TEST( TestComputeInertialTensorFromGroupFromDefaultValue );
  CPPUNIT_TEST( TestComputeInertialTensorFromGroupFromDENSITYTag );
  CPPUNIT_TEST( TestOpDoUndo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestAddAttributes();
  void TestComputeInertialTensorFromDefaultValue();
  void TestComputeInertialTensorFromDENSITYTag();
  void TestComputeInertialTensorFromGroupFromDefaultValue();
  void TestComputeInertialTensorFromGroupFromDENSITYTag();
  void TestOpDoUndo();
  
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
  runner.addTest( medOpComputeInertialTensorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
