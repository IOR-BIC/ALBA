/*=========================================================================

 Program: MAF2Medical
 Module: medVect3dTest
 Authors: Grazia Di Cosmo
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medVect3dTest_H__
#define __CPP_UNIT_medVect3dTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: medVect3dTest
  Test class for medVect3d
*/
class medVect3dTest : public CPPUNIT_NS::TestFixture
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( medVect3dTest );

  /** macro for test TestSetGetVector */
	CPPUNIT_TEST( TestSetGetVector );

  /** macro for test TestOperation */
	CPPUNIT_TEST( TestOperators );

  /** macro for test TestOperation2Vector */
  CPPUNIT_TEST( TestOperation2Vector );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Set and Get vector */
  void TestSetGetVector();
  /** Test arithmetical operators between vector */
	void TestOperators();
  /** Test Operation between vectors */
  void TestOperation2Vector();
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
	runner.addTest( medVect3dTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
