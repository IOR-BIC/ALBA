/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: EdgeIndexTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_EdgeIndexTest_H__
#define __CPP_UNIT_EdgeIndexTest_H__

#include "albaTest.h"

class EdgeIndexTest : public albaTest
{
  CPPUNIT_TEST_SUITE( EdgeIndexTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
};

#endif