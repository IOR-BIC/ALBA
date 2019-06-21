/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: CSkeletonVertexM2Test
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CSkeletonVertexM2TEST_H__
#define __CPP_UNIT_CSkeletonVertexM2TEST_H__

#include "albaTest.h"

class CSkeletonVertexM2Test : public albaTest
{
  CPPUNIT_TEST_SUITE( CSkeletonVertexM2Test );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestIsInPositiveHalfspace );
  CPPUNIT_TEST( TestGetDegree );
  CPPUNIT_TEST_SUITE_END();

protected:
  /** Test Dinamyc allocation of CSkeletonVertex class.*/
  void TestDynamicAllocation();
  /** Test IsInPositiveHalfSpace() method. */
  void TestIsInPositiveHalfspace();
  /** Test GetDegree() method. */
  void TestGetDegree();
};

#endif