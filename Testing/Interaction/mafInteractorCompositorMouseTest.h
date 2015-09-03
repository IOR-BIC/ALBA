/*=========================================================================

 Program: MAF2
 Module: mafInteractorCompositorMouseTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorCompositorMouseTest_H__
#define __CPP_UNIT_mafInteractorCompositorMouseTest_H__

#include "mafTest.h"

class mafInteractorCompositorMouseTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafInteractorCompositorMouseTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestStartInteraction);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestCreateBehavior);
  CPPUNIT_TEST(TestGetBehavior);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestStartInteraction();
    void TestOnEvent();
    void TestCreateBehavior();
    void TestGetBehavior();    
};

#endif
