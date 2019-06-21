/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: MeshDataTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MeshDataTest_H__
#define __CPP_UNIT_MeshDataTest_H__

#include "albaTest.h"

class MeshDataTest : public albaTest
{
  CPPUNIT_TEST_SUITE( MeshDataTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllocateVertices );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAllocateVertices();

};

#endif