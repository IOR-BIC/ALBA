/*=========================================================================

 Program: MAF2
 Module: TriangleTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_TriangleTest_H__
#define __CPP_UNIT_TriangleTest_H__

#include "mafTest.h"

class TriangleTest : public mafTest
{
  CPPUNIT_TEST_SUITE( TriangleTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestArea );
  CPPUNIT_TEST( TestAspectRatio );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestArea();
  void TestAspectRatio();
};

#endif