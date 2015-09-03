/*=========================================================================

 Program: MAF2
 Module: CoredPointIndexTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_CoredPointIndexTest_H__
#define __CPP_UNIT_CoredPointIndexTest_H__

#include "mafTest.h"

class CoredPointIndexTest : public mafTest
{
  CPPUNIT_TEST_SUITE( CoredPointIndexTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST( TestDifferent );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestEquals();
  void TestDifferent();
};

#endif