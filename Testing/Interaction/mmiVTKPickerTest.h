/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiVTKPickerTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiVTKPickerTest_H__
#define __CPP_UNIT_mmiVTKPickerTest_H__

#include "albaTest.h"

class mmiVTKPickerTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( mmiVTKPickerTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestSetGetContinuousPicking );
  CPPUNIT_TEST( TestOnMouseMove );
  CPPUNIT_TEST( TestOnLeftButtonDown );
  CPPUNIT_TEST( TestOnLeftButtonUp );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();
	void TestSetGetContinuousPicking();
	void TestOnMouseMove();
    void TestOnLeftButtonDown();
	void TestOnLeftButtonUp();

};

#endif
