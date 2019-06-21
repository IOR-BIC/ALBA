/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiInfoImageTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiInfoImageTest_H__
#define __CPP_UNIT_mmiInfoImageTest_H__

#include "albaTest.h"

class mmiInfoImageTest : public albaTest
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
