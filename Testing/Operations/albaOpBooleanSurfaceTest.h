/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpBooleanSurfaceTest
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpBooleanSurfaceTest_H
#define CPP_UNIT_albaOpBooleanSurfaceTest_H

#include "albaTest.h"

class albaOpBooleanSurfaceTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpBooleanSurfaceTest );
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
