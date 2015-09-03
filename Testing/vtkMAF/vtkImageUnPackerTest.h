/*=========================================================================

 Program: MAF2
 Module: vtkImageUnPackerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkImageUnPackerTest_H__
#define __CPP_UNIT_vtkImageUnPackerTest_H__

#include "mafTest.h"

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkImageUnPacker;

class vtkImageUnPackerTest : public mafTest
{
public:
  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( vtkImageUnPackerTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestExecute );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestExecute();

  vtkImageUnPacker *m_Filter;

};

#endif