/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFillHolesTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpFillHolesTest_H
#define CPP_UNIT_albaOpFillHolesTest_H

#include "albaTest.h"

class albaOpFillHolesTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpFillHolesTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestOpRun();

};

#endif
