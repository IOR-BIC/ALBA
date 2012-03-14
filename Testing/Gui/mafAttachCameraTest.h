/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAttachCameraTest.h,v $
Language:  C++
Date:      $Date: 2009-03-10 14:54:10 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafAttachCameraTest_H__
#define __CPP_UNIT_mafAttachCameraTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkDataSet;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

class mafVMESurface;
class mafVMERoot;

/** Test for mafAttachCamera */
class mafAttachCameraTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafAttachCameraTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEnableDisableAttachCamera);
  CPPUNIT_TEST(TestSetGetEnableAttachCamera);
  CPPUNIT_TEST(TestSetGetVme);
  CPPUNIT_TEST(TestSetGetStartingMatrix);
  CPPUNIT_TEST(TestSetGetListener);
  CPPUNIT_TEST(TestRenderStuff);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestRenderStuff();
  void TestConstructorDestructor();
  void TestEnableDisableAttachCamera();
  void TestSetGetEnableAttachCamera();
  void TestSetGetVme();
  void TestSetGetStartingMatrix();
  void TestSetGetListener();  

  void CreateRenderStuff();
  void RenderVMESurface( mafVMESurface *vme );
  void CreateTestData();
   
  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;
  
  mafVMESurface *m_TestSurface;
  mafVMERoot *m_Root;

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
  runner.addTest( mafAttachCameraTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
