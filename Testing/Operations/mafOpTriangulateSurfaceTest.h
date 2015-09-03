/*=========================================================================

 Program: MAF2
 Module: mafOpTriangulateSurfaceTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpTriangulateSurfaceTest_H
#define CPP_UNIT_mafOpTriangulateSurfaceTest_H

#include "mafTest.h"

class mafOpTriangulateSurfaceTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpTriangulateSurfaceTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpRun();
};

#endif
