/*=========================================================================

 Program: MAF2
 Module: SparseMatrixTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_SparseMatrixTest_H__
#define __CPP_UNIT_SparseMatrixTest_H__

#include "mafTest.h"

class SparseMatrixTest : public mafTest
{
  CPPUNIT_TEST_SUITE( SparseMatrixTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetAllocator );
  CPPUNIT_TEST( TestResize );
  CPPUNIT_TEST( TestSetRowSize );
  CPPUNIT_TEST( TestSetZero );
  CPPUNIT_TEST( TestSetIdentity );
  CPPUNIT_TEST( TestOperator1 );
  CPPUNIT_TEST( TestOperator2 );
  CPPUNIT_TEST( TestOperator3 );
  CPPUNIT_TEST( TestMultiplyMatrix );
  CPPUNIT_TEST( TestMultiplyTransposeMatrix );
  CPPUNIT_TEST( TestMultiplyVector1 );
  CPPUNIT_TEST( TestMultiplyVector2 );
  CPPUNIT_TEST( TestTranspose );
  CPPUNIT_TEST( TestSolve );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSetAllocator();
  void TestResize();
  void TestSetRowSize();
  void TestSetZero();
  void TestSetIdentity();
  void TestOperator1();
  void TestOperator2();
  void TestOperator3();
  void TestMultiplyMatrix();
  void TestMultiplyTransposeMatrix();
  void TestMultiplyVector1();
  void TestMultiplyVector2();
  void TestTranspose();
  void TestSolve();
};

#endif