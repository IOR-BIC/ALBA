/*=========================================================================

 Program: MAF2
 Module: mafLODActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafLODActortest_H__
#define __CPP_UNIT_mafLODActortest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class mafLODActor;

//------------------------------------------------------------------------------
// Test class for mafLODActor
//------------------------------------------------------------------------------
class mafLODActorTest : public CPPUNIT_NS::TestFixture
{
  public:
    enum ID_TEST_LIST
    {
      ID_FLAG_DIMENSION = 0,
	    ID_PIXEL_THRESHOLD,
			ID_ENABLE_FADING,
			ID_ENABLE_HIGH_THRESHOLD,
    };

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafLODActorTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
		CPPUNIT_TEST( TestPixelThreshold);
		CPPUNIT_TEST( TestFlagDimension );
		CPPUNIT_TEST( TestEnableFading );
		CPPUNIT_TEST( TestEnableHighThreshold);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestFixture();
    void TestDynamicAllocation();
		void TestPixelThreshold();
		void TestFlagDimension();
    void TestEnableFading();
		void TestEnableHighThreshold();

    //accessories
    void RenderData(mafLODActor *actor);
    
    int m_TestNumber;
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
  runner.addTest( mafLODActorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
