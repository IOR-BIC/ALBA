/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: CSkeletonEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CSkeletonEdgeTEST_H__
#define __CPP_UNIT_CSkeletonEdgeTEST_H__

#include "albaTest.h"

class CSkeletonEdgeTest : public albaTest
{
  CPPUNIT_TEST_SUITE( CSkeletonEdgeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetNumberOfConnectedEdges );
  CPPUNIT_TEST( TestIsInternal );
  CPPUNIT_TEST( TestGetLength );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetNumberOfConnectedEdges();
  void TestIsInternal();
  void TestGetLength();
};

#endif