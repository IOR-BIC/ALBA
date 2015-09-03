/*=========================================================================

 Program: MAF2
 Module: TreeNodeDataTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_TreeNodeDataTEST_H__
#define __CPP_UNIT_TreeNodeDataTEST_H__

#include "mafTest.h"

class TreeNodeDataTest : public mafTest
{
  CPPUNIT_TEST_SUITE( TreeNodeDataTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();

};

#endif