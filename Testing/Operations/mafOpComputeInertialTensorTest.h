/*=========================================================================

 Program: MAF2
 Module: mafOpComputeInertialTensorTest
 Authors: Simone Brazzale , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpComputeInertialTensorTest_H
#define CPP_UNIT_mafOpComputeInertialTensorTest_H

#include "mafTest.h"

class mafOpComputeInertialTensorTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpComputeInertialTensorTest );
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
