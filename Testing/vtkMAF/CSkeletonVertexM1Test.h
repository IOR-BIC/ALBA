/*=========================================================================

 Program: MAF2
 Module: CSkeletonVertexM1Test
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CSkeletonVertexM1Test_H__
#define __CPP_UNIT_CSkeletonVertexM1Test_H__

#include "mafTest.h"

class CSkeletonVertexM1Test : public mafTest
{
  CPPUNIT_TEST_SUITE( CSkeletonVertexM1Test );
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