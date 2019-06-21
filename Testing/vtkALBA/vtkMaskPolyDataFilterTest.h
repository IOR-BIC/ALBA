/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkMaskPolyDataFilterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_vtkMaskPolyDataFilterTEST_H
#define CPP_UNIT_vtkMaskPolyDataFilterTEST_H

#include "albaTest.h"

class vtkMaskPolyDataFilterTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkMaskPolyDataFilterTest );
  CPPUNIT_TEST( TestStructuredPoints );
	CPPUNIT_TEST( TestRectilinearGrid );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestStructuredPoints();
    void TestRectilinearGrid();
};

#endif
