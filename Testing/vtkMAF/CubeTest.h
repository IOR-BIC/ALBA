/*=========================================================================

 Program: MAF2
 Module: CubeTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CubeTest_H__
#define __CPP_UNIT_CubeTest_H__

#include "mafTest.h"

class CubeTest : public mafTest
{
  CPPUNIT_TEST_SUITE( CubeTest );
  
  CPPUNIT_TEST( TestCornerIndex );
  CPPUNIT_TEST( FactorCornerIndexTest );

  CPPUNIT_TEST( EdgeIndexTest );
  CPPUNIT_TEST( FactorEdgeIndexTest );
  CPPUNIT_TEST( FaceIndexTest );

  CPPUNIT_TEST( FactorFaceIndexTest );
  CPPUNIT_TEST( FaceAdjacentToEdgesTest );
  CPPUNIT_TEST( EdgeCornersTest );
  CPPUNIT_TEST( FaceCornersTest );

  CPPUNIT_TEST( FaceReflectEdgeIndexTest );
  CPPUNIT_TEST( FaceReflectCornerIndexTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestCornerIndex();
  void FactorCornerIndexTest();

  void EdgeIndexTest();
  void FactorEdgeIndexTest();
  void FaceIndexTest();

  void FactorFaceIndexTest();
  void FaceAdjacentToEdgesTest();
  void EdgeCornersTest();
  void FaceCornersTest();

  void FaceReflectEdgeIndexTest();
  void FaceReflectCornerIndexTest();

};

#endif