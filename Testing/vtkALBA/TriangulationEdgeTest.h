/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: TriangulationEdgeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_TriangulationEdgeTest_H__
#define __CPP_UNIT_TriangulationEdgeTest_H__

#include "albaTest.h"

class TriangulationEdgeTest : public albaTest
{
  CPPUNIT_TEST_SUITE( TriangulationEdgeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
};

#endif