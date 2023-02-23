/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensityTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MMOVOIDENSITYTEST_H__
#define __CPP_UNIT_MMOVOIDENSITYTEST_H__

#include "albaTest.h"

class albaOpVOIDensityTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpVOIDensityTest );
  CPPUNIT_TEST( Test );
	CPPUNIT_TEST( TestNotAlignedVolume );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
		void TestNotAlignedVolume();
};

#endif
