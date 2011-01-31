/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpRegisterClustersTest.h,v $
Language:  C++
Date:      $Date: 2011-01-31 10:37:49 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medOpRegisterClustersTest_H__
#define __CPP_UNIT_medOpRegisterClustersTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpRegisterClustersTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medOpRegisterClustersTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  //CPPUNIT_TEST( OnEventTest );
  CPPUNIT_TEST( CopyTest );
  CPPUNIT_TEST( AcceptTest );   
  CPPUNIT_TEST( OpRunTest );
  CPPUNIT_TEST( OpDoUndoTest );
  CPPUNIT_TEST( ClosedCloudAcceptTest );
  CPPUNIT_TEST( SurfaceAcceptTest );
  CPPUNIT_TEST_SUITE_END();

protected:

    void DynamicAllocationTest();
    //void OnEventTest(); Only GUI events. not tested
    void CopyTest();
    void AcceptTest();   
    void OpRunTest();
    void OpDoUndoTest();
    void ClosedCloudAcceptTest();
    void SurfaceAcceptTest();

};

int main( int argc, char* argv[] )
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
  runner.addTest( medOpRegisterClustersTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
