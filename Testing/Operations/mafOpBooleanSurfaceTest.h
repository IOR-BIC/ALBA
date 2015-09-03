/*=========================================================================

 Program: MAF2
 Module: mafOpBooleanSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpBooleanSurfaceTest_H
#define CPP_UNIT_mafOpBooleanSurfaceTest_H

#include "mafTest.h"

class mafOpBooleanSurfaceTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpBooleanSurfaceTest );
  CPPUNIT_TEST( TestUnion );
  CPPUNIT_TEST( TestIntersection );
  CPPUNIT_TEST( TestDifference );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestIntersection();
		void TestUnion();
		void TestDifference();
};

#endif
