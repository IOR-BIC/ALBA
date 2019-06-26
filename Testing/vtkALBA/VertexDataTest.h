/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: VertexDataTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VertexDataTest_H__
#define __CPP_UNIT_VertexDataTest_H__

#include "albaTest.h"

class VertexDataTest : public albaTest
{
  CPPUNIT_TEST_SUITE( VertexDataTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCylCoord );
  CPPUNIT_TEST ( TestCartCoords );
  CPPUNIT_TEST ( TestId ) ;
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCylCoord();
  void TestCartCoords();
  void TestId();

};

#endif