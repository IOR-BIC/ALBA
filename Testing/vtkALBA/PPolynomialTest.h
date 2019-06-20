/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: PPolynomialTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_PPolynomialTest_H__
#define __CPP_UNIT_PPolynomialTest_H__

#include "albaTest.h"

class PPolynomialTest : public albaTest
{
  CPPUNIT_TEST_SUITE( PPolynomialTest );
  CPPUNIT_TEST( TestAllMethods );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestAllMethods();
  
};

#endif