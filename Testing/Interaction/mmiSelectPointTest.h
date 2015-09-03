/*=========================================================================

 Program: MAF2
 Module: mmiSelectPointTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiSelectPointTest_H__
#define __CPP_UNIT_mmiSelectPointTest_H__

#include "mafTest.h"

class mmiSelectPointTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mmiSelectPointTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestOnMouseMove );
  CPPUNIT_TEST( TestOnLeftButtonDown );
  CPPUNIT_TEST( TestOnButtonUp );
  CPPUNIT_TEST( TestSetCtrlModifierOnOff );
  CPPUNIT_TEST( TestSetGetCtrlModifier );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();
    void TestOnMouseMove();
    void TestOnLeftButtonDown();
  	void TestOnButtonUp();
    void TestSetCtrlModifierOnOff();
	  void TestSetGetCtrlModifier();
};

#endif
