/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAErythrocyteSourceTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_vtkALBAErythrocyteSourceTEST_H
#define CPP_UNIT_vtkALBAErythrocyteSourceTEST_H

#include "albaTest.h"

class vtkALBAErythrocyteSourceTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkALBAErythrocyteSourceTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestExecuteData );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestDynamicAllocation();
    void TestExecuteData();
};

#endif
