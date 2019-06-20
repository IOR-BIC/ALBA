/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSliceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeSliceTest_H__
#define __CPP_UNIT_albaPipeSliceTest_H__

#include "albaTest.h"

class albaPipeSliceTest : public albaTest
{
public: 

	CPPUNIT_TEST_SUITE( albaPipeSliceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestGetOrigin1 ); 
  CPPUNIT_TEST( TestGetOrigin2 );
  CPPUNIT_TEST( TestGetNormal1 );
  CPPUNIT_TEST( TesGetNormal2 );
  CPPUNIT_TEST( TestGetSlice );   
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestGetOrigin1(); 
  void TestGetOrigin2();
  void TestGetNormal1();
  void TesGetNormal2();
  void TestGetSlice();  

};

#endif
