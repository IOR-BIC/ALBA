/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDynamicMatrixTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaDynamicMatrixTest_H
#define CPP_UNIT_albaDynamicMatrixTest_H

#include "albaTest.h"

class albaDynamicMatrixTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaDynamicMatrixTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	//CPPUNIT_TEST(TestReadMatrix);
	CPPUNIT_TEST(TestBigFileRead);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestDynamicAllocation();
  void TestReadMatrix();
	void TestBigFileRead();
};

#endif
