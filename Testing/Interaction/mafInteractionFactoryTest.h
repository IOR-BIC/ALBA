/*=========================================================================

 Program: MAF2
 Module: mafInteractionFactoryTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#endif
