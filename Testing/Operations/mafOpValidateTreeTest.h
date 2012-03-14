/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpValidateTreeTest.h,v $
Language:  C++
Date:      $Date: 2009-10-15 08:38:53 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MMOVOIDENSITYTEST_H__
#define __CPP_UNIT_MMOVOIDENSITYTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


/**
  Class Name: mafOpValidateTreeTest
  Test class for mafOpValidateTree.
*/
class mafOpValidateTreeTest : public CPPUNIT_NS::TestFixture
{
  /** test suite starting */
  CPPUNIT_TEST_SUITE( mafOpValidateTreeTest );
  /** test OpRun */
  CPPUNIT_TEST( TestOpRun );
  /** test Accept */
  CPPUNIT_TEST( TestAccept );
  /** test Tree Validation with correct tree */
  CPPUNIT_TEST( TestValidateTree_ValidTree );
  /** test Tree Validation with invalid node */
  CPPUNIT_TEST( TestValidateTree_InvalidNode );
  /** test Tree Validation with link not present */
  CPPUNIT_TEST( TestValidateTree_LinkNotPresent );
  /** test Tree Validation with link null */
  CPPUNIT_TEST( TestValidateTree_LinkNull );
  /** test Tree Validation with link binary file not present */
  CPPUNIT_TEST( TestValidateTree_BinaryFileNotPresent );
  /** test Tree Validation with empty url */
  CPPUNIT_TEST( TestValidateTree_UrlEmpty );
  /** test Tree Validation with item not present */
  CPPUNIT_TEST( TestValidateTree_ItemNotPresent ); //deprecated , see function body for explanation
  /** test Tree Validation with not coherent max item id  */
  CPPUNIT_TEST( TestValidateTree_MaxItemIdPatched );
  /** test Tree Validation with archive not present */
  CPPUNIT_TEST( TestValidateTree_ArchiveFileNotPresent );
  
  /** test suite ending  */
  CPPUNIT_TEST_SUITE_END();

  protected:
    /** test OpRun */
    void TestOpRun();
    /** test Accept */
    void TestAccept();
    /** test Tree Validation with correct tree */
    void TestValidateTree_ValidTree();
    /** test Tree Validation with invalid node */
    void TestValidateTree_InvalidNode();
    /** test Tree Validation with link not present */
    void TestValidateTree_LinkNotPresent();
    /** test Tree Validation with link null */
    void TestValidateTree_LinkNull();
    /** test Tree Validation with link binary file not present */
    void TestValidateTree_BinaryFileNotPresent();
    /** test Tree Validation with empty url */
    void TestValidateTree_UrlEmpty();
    /** test Tree Validation with item not present */
    void TestValidateTree_ItemNotPresent();
    /** test Tree Validation with not coherent max item id  */
    void TestValidateTree_MaxItemIdPatched();
    /** test Tree Validation with archive not present */
    void TestValidateTree_ArchiveFileNotPresent();
    
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
  runner.addTest( mafOpValidateTreeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
