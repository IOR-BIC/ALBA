/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeInertialTensorTest
 Authors: Simone Brazzale , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpComputeInertialTensorTest_H
#define CPP_UNIT_albaOpComputeInertialTensorTest_H

#include "albaTest.h"

class albaOpComputeInertialTensorTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpComputeInertialTensorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestAddAttributes );
  CPPUNIT_TEST( TestComputeInertialTensorFromDefaultValue );
  CPPUNIT_TEST( TestComputeInertialTensorFromDENSITYTag );
  CPPUNIT_TEST( TestComputeInertialTensorFromGroupFromDefaultValue );
  CPPUNIT_TEST( TestComputeInertialTensorFromGroupFromDENSITYTag );
  CPPUNIT_TEST( TestOpDoUndo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestAddAttributes();
  void TestComputeInertialTensorFromDefaultValue();
  void TestComputeInertialTensorFromDENSITYTag();
  void TestComputeInertialTensorFromGroupFromDefaultValue();
  void TestComputeInertialTensorFromGroupFromDENSITYTag();
  void TestOpDoUndo();
  
};

#endif
