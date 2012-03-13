/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTransformFrameTest.h,v $
Language:  C++
Date:      $Date: 2008-02-29 15:03:38 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafTransformFrameTest_H__
#define __CPP_UNIT_mafTransformFrameTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkTransform;
class mafMatrix;

/** Test suite for mafTransformFrame */
class mafTransformFrameTest : public CPPUNIT_NS::TestFixture
{
  public: 

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( mafTransformFrameTest );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestMafTransformFrameConstructorDestructor );
    CPPUNIT_TEST( TestSetInputMafTransformBase );
    CPPUNIT_TEST( TestSetInputMafMatrix );
    CPPUNIT_TEST( TestGetInput );
    CPPUNIT_TEST( TestSetInputFrameMafMatrix );
    CPPUNIT_TEST( TestSetInputFrameMafTransformBase );
    CPPUNIT_TEST( TestGetInputFrame );
    CPPUNIT_TEST( TestSetTargetFrameMafMatrix );
    CPPUNIT_TEST( TestSetTargetFrameMafTransformBase );
    CPPUNIT_TEST( TestGetTargetFrame );
    CPPUNIT_TEST( TestGetMTime );
    CPPUNIT_TEST( TestMatrixTransformationFromInputFrameToTargetFrameWithRendering );
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();
    void TestMatrixTransformationFromInputFrameToTargetFrameWithRendering();

    void TestMafTransformFrameConstructorDestructor();
    
    /** set the materix to be transformed */
    void TestSetInputMafTransformBase();
    void TestSetInputMafMatrix();
    void TestGetInput();

    /**
    Set/Get the input reference system, i.e. the reference system of the 
    input matrix.*/
    void TestSetInputFrameMafMatrix();
    void TestSetInputFrameMafTransformBase();
    void TestGetInputFrame();

    /**
    Set/Get the output reference system, i.e. the reference system of the output
    matrix or the target reference system for point transformation.*/
    void TestSetTargetFrameMafMatrix();
    void TestSetTargetFrameMafTransformBase();
    void TestGetTargetFrame();

    /** 
    Return current modification time, taking inro consideration also
    Input, InputFrame and TargetFrame. */
    void TestGetMTime();


    vtkTransform *m_InputFrameTransform;
    vtkTransform *m_TargetFrameTransform;
    vtkTransform *m_InputMatrixTransform;
    mafMatrix *m_InputMatrix;
    mafMatrix *m_InputFrameMatrix;
    mafMatrix *m_TargetFrameMatrix;

    

};


int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafTransformFrameTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
