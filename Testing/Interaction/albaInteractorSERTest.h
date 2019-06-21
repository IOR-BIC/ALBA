/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSERTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorSERTest_H__
#define __CPP_UNIT_albaInteractorSERTest_H__

#include "albaTest.h"

class albaInteractorSERTest : public albaTest
{
public:
  

  CPPUNIT_TEST_SUITE( albaInteractorSERTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestBindDeviceToAction );
  CPPUNIT_TEST( TestUnBindDeviceFromAction );
  CPPUNIT_TEST( TestUnBindDeviceFromAllActions);
  CPPUNIT_TEST( TestGetDeviceBindings);
  CPPUNIT_TEST( TestBindAction);
  CPPUNIT_TEST( TestUnBindAction);
  CPPUNIT_TEST( TestAddAction);
  CPPUNIT_TEST( TestGetAction);
  CPPUNIT_TEST( TestGetActions);
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructorDestructor();
  void TestBindDeviceToAction();
  void TestUnBindDeviceFromAction();
  void TestUnBindDeviceFromAllActions();
  void TestGetDeviceBindings();
  void TestBindAction();
  void TestUnBindAction();
  void TestAddAction();
  void TestGetAction();
  void TestGetActions();

};

#endif
