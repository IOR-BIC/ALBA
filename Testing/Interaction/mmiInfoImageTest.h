/*=========================================================================

 Program: MAF2
 Module: mmiInfoImageTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiInfoImageTest_H__
#define __CPP_UNIT_mmiInfoImageTest_H__

#include "mafTest.h"

class mmiInfoImageTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mmiInfoImageTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestOnEvent );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
	void TestFixture();
	void TestConstructorDestructor();
	void TestOnEvent();

};

#endif
