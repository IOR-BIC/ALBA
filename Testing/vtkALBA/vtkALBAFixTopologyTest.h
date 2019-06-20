/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixTopologyTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAFixTopologyTEST_H__
#define __CPP_UNIT_vtkALBAFixTopologyTEST_H__

#include "albaTest.h"

class vtkALBAFixTopologyTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkALBAFixTopologyTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestExecute );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestExecute();
};

#endif