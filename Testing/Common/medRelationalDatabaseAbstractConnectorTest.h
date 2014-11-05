/*=========================================================================

 Program: MAF2Medical
 Module: medRelationalDatabaseAbstractConnectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medRelationalDatabaseAbstractConnectorTest_H__
#define __CPP_UNIT_medRelationalDatabaseAbstractConnectorTest_H__

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
class name: medRelationalDatabaseAbstractConnectorTest
  Test class for medRelationalDatabaseAbstractConnector
*/
class medRelationalDatabaseAbstractConnectorTest : public CPPUNIT_NS::TestFixture
{
public:

	/** CPPUNIT fixture: executed before each test */
	void setUp();

	/** CPPUNIT fixture: executed after each test */
	void tearDown();

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( medRelationalDatabaseAbstractConnectorTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );

  /** macro for test TestSetGetRelationalDatabaseInformation */
  CPPUNIT_TEST( TestSetGetRelationalDatabaseInformation );

  /** macro for test TestSetGetQuery */
  CPPUNIT_TEST( TestSetGetQuery );

  /** macro for test TestIsFailed */
  CPPUNIT_TEST( TestIsFailed );

  /** macro for test TestGetNumberOfRecords */
  CPPUNIT_TEST( TestGetNumberOfRecords );

  /** macro for test TestGetNumberOfFields */
  CPPUNIT_TEST( TestGetNumberOfFields );

  /** macro for test TestGetQueryResultAsStringMatrix */
  CPPUNIT_TEST( TestGetQueryResultAsStringMatrix );

  /** macro for test TestGetColumnsTypeAsStringVector */
  CPPUNIT_TEST( TestGetColumnsTypeAsStringVector );

  /** macro for test TestGetColumnsNameAsStringVector */
  CPPUNIT_TEST( TestGetColumnsNameAsStringVector );

  /** macro for test TestGetQueryResultAsObjectsMatrix */
  CPPUNIT_TEST( TestGetQueryResultAsObjectsMatrix );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();

  /** test method for SetRelationalDatabaseInformation GetRelationalDatabaseInformation*/
  void TestSetGetRelationalDatabaseInformation();

  /** test method for SetQuery & GetQuery*/
  void TestSetGetQuery();

  /** test method for IsFailed*/
  void TestIsFailed();

  /** test method for GetNumberOfRecords*/
  void TestGetNumberOfRecords();

  /** test method for GetNumberOfFields*/
  void TestGetNumberOfFields();

  /** test method for GetQueryResultAsStringMatrix*/
  void TestGetQueryResultAsStringMatrix();

  /** test method for GetColumnsTypeAsStringVector*/
  void TestGetColumnsTypeAsStringVector();

  /** test method for GetColumnsNameAsStringVector*/
  void TestGetColumnsNameAsStringVector();

  /** test method for GetQueryResultAsObjectsMatrix*/
  void TestGetQueryResultAsObjectsMatrix();

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
	runner.addTest( medRelationalDatabaseAbstractConnectorTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
