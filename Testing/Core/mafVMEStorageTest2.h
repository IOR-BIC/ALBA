/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEStorageTest2.h,v $
Language:  C++
Date:      $Date: 2008-07-03 11:32:08 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEStorageTest2_H__
#define __CPP_UNIT_mafVMEStorageTest2_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"

#include "vtkSphereSource.h"
#include "vtkConeSource.h"

/** Test used to isolate leaks in storage related classes; see mafVMEStorageTest for a store/restore
example with synthetic generated time varying data*/
class mafVMEStorageTest2 : public CPPUNIT_NS::TestFixture
{
  public: 

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( mafVMEStorageTest2 );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestStoreRestoreTagArray );
    CPPUNIT_TEST(TestRestoreOnNewStorage);
    CPPUNIT_TEST(TestRestoreOnSameStorage);
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();

    void TestRestoreOnNewStorage();
    void TestRestoreOnSameStorage();
    void TestStoreRestoreTagArray();

    //create a storage: this also creates a root
    mafVMEStorage m_Storage1;
    mafVMERoot *m_Storage1Root;

    // helper objects
    vtkSphereSource *m_SphereSource;
    vtkConeSource *m_ConeSource;
    mafTimeStamp m_CurrentTime;

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
  runner.addTest( mafVMEStorageTest2::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
