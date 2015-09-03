/*=========================================================================

 Program: MAF2
 Module: StartingPolynomialTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_StartingPolynomialTest_H__
#define __CPP_UNIT_StartingPolynomialTest_H__

#include "mafTest.h"

class StartingPolynomialTest : public mafTest
{
  CPPUNIT_TEST_SUITE( StartingPolynomialTest );
  CPPUNIT_TEST( TestAllMethods );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestAllMethods();
  
};

#endif