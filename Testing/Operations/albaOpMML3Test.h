/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3Test
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpMML3Test_H__
#define __CPP_UNIT_albaOpMML3Test_H__

#include "albaTest.h"

#include "vtkDataSet.h"
#include "vtkTransform.h"

class albaOpMML3Test : public albaTest
{
public:
  CPPUNIT_TEST_SUITE( albaOpMML3Test );
  CPPUNIT_TEST( TestDynamicAllocation);
  CPPUNIT_TEST( TestOpDo);
  CPPUNIT_TEST( TestOpUndo);
  CPPUNIT_TEST( TestAccept);
  CPPUNIT_TEST( TestCopy);
  CPPUNIT_TEST( TestAcceptVMELandmark);
  CPPUNIT_TEST( TestAcceptVMESurface);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpDo();
  void TestOpUndo();
  void TestAccept();
  void TestCopy();
  void TestAcceptVMELandmark();
  void TestAcceptVMESurface();
};

#endif
