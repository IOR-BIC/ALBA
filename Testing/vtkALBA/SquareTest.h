/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: SquareTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_SquareTest_H__
#define __CPP_UNIT_SquareTest_H__

#include "albaTest.h"

class SquareTest : public albaTest
{
  CPPUNIT_TEST_SUITE( SquareTest );
  CPPUNIT_TEST( TestCornerIndex );
  CPPUNIT_TEST( FactorCornerIndexTest );
  CPPUNIT_TEST( EdgeIndexTest );
  CPPUNIT_TEST( FactorEdgeIndexTest );
  CPPUNIT_TEST( EdgeCornersTest );
  CPPUNIT_TEST( ReflectEdgeIndexTest );
  CPPUNIT_TEST( ReflectCornerIndexTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestCornerIndex();
  void FactorCornerIndexTest();

  void EdgeIndexTest();
  void FactorEdgeIndexTest();

  void EdgeCornersTest();

  void ReflectEdgeIndexTest();
  void ReflectCornerIndexTest();

};

#endif