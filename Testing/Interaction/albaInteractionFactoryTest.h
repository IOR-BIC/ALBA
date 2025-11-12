/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionFactoryTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractionFactoryTEST_H__
#define __CPP_UNIT_albaInteractionFactoryTEST_H__

#include "albaTest.h"


class albaInteractionFactoryTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaInteractionFactoryTest );
  
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
  CPPUNIT_TEST( TestGetDescription );

  CPPUNIT_TEST_SUITE_END();

protected:
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
