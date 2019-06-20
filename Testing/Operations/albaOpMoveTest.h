/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMoveTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpMoveTEST_H
#define CPP_UNIT_albaOpMoveTEST_H

#include "albaTest.h"

class albaOpMoveTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpMoveTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
  void TestReset();
};

#endif
