/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAvatarTest.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafAvatarTest_H__
#define __CPP_UNIT_mafAvatarTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "mafAvatar.h"

class mafAvatarTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    /** CPPUNIT fixture: executed before each test */
    void setUp();

    /** CPPUNIT fixture: executed after each test */
    void tearDown();

    CPPUNIT_TEST_SUITE( mafAvatarTest );
    
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestSetGetView);
    CPPUNIT_TEST(TestGetActor3D);
    CPPUNIT_TEST(TestPick);
    CPPUNIT_TEST(TestOnEvent);
    CPPUNIT_TEST(TestSetGetTracker);
    CPPUNIT_TEST(TestSetGetMode);
    CPPUNIT_TEST(TestSetModeTo2D);
    CPPUNIT_TEST(TestSetModeTo3D);
    CPPUNIT_TEST(TestGetPicker);
    CPPUNIT_TEST_SUITE_END();
    
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestSetGetView();
    void TestGetActor3D();
    void TestPick();
    void TestOnEvent();
    void TestSetGetTracker();
    void TestSetGetMode();
    void TestSetModeTo2D();
    void TestSetModeTo3D();
    void TestGetPicker();
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
  runner.addTest( mafAvatarTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

class ConcreteMafAvatar : public mafAvatar
{
public:

  mafTypeMacro(ConcreteMafAvatar,mafAvatar);

  void OnEvent(mafEventBase *maf_event);
  int GetLastReceivedEventID();

protected:
  
  ConcreteMafAvatar();
  virtual ~ConcreteMafAvatar();

private:

  ConcreteMafAvatar(const ConcreteMafAvatar&) {}  // Not implemented.
  void operator=(const ConcreteMafAvatar&) {}  // Not implemented.
 
  int m_LastReceivedEventID;
};


#endif
