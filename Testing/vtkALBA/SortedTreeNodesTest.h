/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: SortedTreeNodesTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_SortedTreeNodesTEST_H__
#define __CPP_UNIT_SortedTreeNodesTEST_H__

#include "albaTest.h"

class SortedTreeNodesTest : public albaTest
{
  CPPUNIT_TEST_SUITE( SortedTreeNodesTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSet );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSet();

};

#endif