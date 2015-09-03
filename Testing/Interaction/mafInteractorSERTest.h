/*=========================================================================

 Program: MAF2
 Module: mafInteractorSERTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorSERTest_H__
#define __CPP_UNIT_mafInteractorSERTest_H__

#include "mafTest.h"

class mafInteractorSERTest : public mafTest
{
public:
  

  CPPUNIT_TEST_SUITE( mafInteractorSERTest );
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
