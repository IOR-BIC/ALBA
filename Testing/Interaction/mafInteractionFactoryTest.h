/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractionFactoryTest.h,v $
Language:  C++
Date:      $Date: 2009-09-16 10:36:36 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafInteractionFactoryTEST_H__
#define __CPP_UNIT_mafInteractionFactoryTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafInteractionFactoryTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafInteractionFactoryTest );
  
  CPPUNIT_TEST( TestGetInstance );
  CPPUNIT_TEST( TestInitialize );
  CPPUNIT_TEST( TestRegisterNewAvatar_CreateAvatarInstance );
  CPPUNIT_TEST( TestGetNumberOfAvatars );
  CPPUNIT_TEST( TestGetAvatarNames );
  CPPUNIT_TEST( TestGetAvatarName );
  CPPUNIT_TEST( TestGetAvatarDescription );
  CPPUNIT_TEST( TestRegisterNewDevice_CreateDeviceInstance );
  CPPUNIT_TEST( TestGetNumberOfDevices );
  CPPUNIT_TEST( TestGetDeviceNames );
  CPPUNIT_TEST( TestGetDeviceName );
  CPPUNIT_TEST( TestGetDeviceDescription );
  CPPUNIT_TEST( TestGetMAFSourceVersion );
  CPPUNIT_TEST( TestGetDescription );

  CPPUNIT_TEST_SUITE_END();

protected:
  void TestGetMAFSourceVersion();
  void TestGetDescription();
  void TestGetInstance();
  void TestInitialize();
  void TestRegisterNewAvatar_CreateAvatarInstance();
  void TestRegisterNewDevice_CreateDeviceInstance();
  void TestGetAvatarName();
  void TestGetAvatarNames();
  void TestGetAvatarDescription();
  void TestGetNumberOfAvatars();
  void TestGetDeviceName();
  void TestGetDeviceNames();
  void TestGetDeviceDescription();
  void TestGetNumberOfDevices();

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
  runner.addTest( mafInteractionFactoryTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
