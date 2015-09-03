/*=========================================================================

 Program: MAF2
 Module: SortedTreeNodesTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_SortedTreeNodesTEST_H__
#define __CPP_UNIT_SortedTreeNodesTEST_H__

#include "mafTest.h"

class SortedTreeNodesTest : public mafTest
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