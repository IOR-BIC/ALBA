/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: CSkeletonVertexTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CSkeletonVertexTest_H__
#define __CPP_UNIT_CSkeletonVertexTest_H__

#include "albaTest.h"

class CSkeletonVertexTest : public albaTest
{
  CPPUNIT_TEST_SUITE( CSkeletonVertexTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetNumberOfJoinedVertices );
  CPPUNIT_TEST( TestIsInPositiveHalfspace );
  CPPUNIT_TEST( TestGetDegree );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetNumberOfJoinedVertices();
  void TestIsInPositiveHalfspace();
  void TestGetDegree();
};

#endif