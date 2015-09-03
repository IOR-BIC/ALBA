/*=========================================================================

 Program: MAF2
 Module: mafOpCropTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_MMOCROPTEST_H
#define CPP_UNIT_MMOCROPTEST_H

#include "mafTest.h"

class mafOpCropTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpCropTest );
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
