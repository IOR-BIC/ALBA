/*=========================================================================

 Program: MAF2
 Module: mmiVTKPickerTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmiVTKPickerTest_H__
#define __CPP_UNIT_mmiVTKPickerTest_H__

#include "mafTest.h"

class mmiVTKPickerTest : public mafTest
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
