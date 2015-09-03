/*=========================================================================

 Program: MAF2
 Module: CSkeletonVertexTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CSkeletonVertexTest_H__
#define __CPP_UNIT_CSkeletonVertexTest_H__

#include "mafTest.h"

class CSkeletonVertexTest : public mafTest
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