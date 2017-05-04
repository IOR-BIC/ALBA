/*=========================================================================

 Program: MAF2
 Module: mafInteractorTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorTest_H__
#define __CPP_UNIT_mafInteractorTest_H__

#include "mafTest.h"

class mafInteractorTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( mafInteractorTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestSetGetRenderer );
  CPPUNIT_TEST( TestSetGetDevice );
  CPPUNIT_TEST( TestSetGetIgnoreTriggerEvents );
  CPPUNIT_TEST( TestSetGetStartInteractionEvent );
  CPPUNIT_TEST( TestSetGetStopInteractionEvent );
  CPPUNIT_TEST( TestSetGetStartButton );
  CPPUNIT_TEST( TestSetGetModifiers );
  CPPUNIT_TEST( TestGetCurrentButton );
  CPPUNIT_TEST( TestGetCurrentModifier );
  CPPUNIT_TEST( TestStartStopInteraction );
  CPPUNIT_TEST( TestIsInteracting );
  CPPUNIT_TEST( TestSetGetButtonMode );
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetRenderer();
  void TestSetGetDevice();
  void TestSetGetIgnoreTriggerEvents();
  void TestSetGetStartInteractionEvent();
  void TestSetGetStopInteractionEvent();
  void TestSetGetStartButton();
  void TestSetGetModifiers();
  void TestGetCurrentButton();
  void TestGetCurrentModifier();
  void TestStartStopInteraction();
  void TestIsInteracting();
  void TestSetGetButtonMode();
};

#endif
