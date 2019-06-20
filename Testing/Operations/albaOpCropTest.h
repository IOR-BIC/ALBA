/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCropTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_MMOCROPTEST_H
#define CPP_UNIT_MMOCROPTEST_H

#include "albaTest.h"

class albaOpCropTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpCropTest );
	CPPUNIT_TEST( DynamicAllocation );
  CPPUNIT_TEST( TestCropRG );
	CPPUNIT_TEST( TestCropSP );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void DynamicAllocation();
    void TestCropRG();
		void TestCropSP();
};

#endif
