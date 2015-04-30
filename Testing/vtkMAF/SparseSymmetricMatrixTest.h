/*=========================================================================

 Program: MAF2Medical
 Module: SparseSymmetricMatrixTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_SparseSymmetricMatrixTest_H__
#define __CPP_UNIT_SparseSymmetricMatrixTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class SparseSymmetricMatrixTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SparseSymmetricMatrixTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestStaticAllocation );
    CPPUNIT_TEST( TestOperatorStar );
    CPPUNIT_TEST( TestMultiplyVector1 );
    CPPUNIT_TEST( TestMultiplyVector2 );
    CPPUNIT_TEST( TestSolve1 );
    CPPUNIT_TEST( TestSolve2 );
    CPPUNIT_TEST_SUITE_END();

protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestOperatorStar();
    void TestMultiplyVector1();
    void TestMultiplyVector2();
    void TestSolve1();
    void TestSolve2();
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
    runner.addTest( SparseSymmetricMatrixTest::suite());
    runner.run( controller );

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
    outputter.write(); 

    return result.wasSuccessful() ? 0 : 1;
}

#endif