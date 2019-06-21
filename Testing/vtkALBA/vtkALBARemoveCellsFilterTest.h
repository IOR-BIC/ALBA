/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARemoveCellsFilterTest
 Authors: Matteo Giacomoni, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBARemoveCellsFilterTEST_H__
#define __CPP_UNIT_vtkALBARemoveCellsFilterTEST_H__

#include "albaTest.h"

class vtkPolyData;

class vtkALBARemoveCellsFilterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBARemoveCellsFilterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestMarkCell );
    CPPUNIT_TEST( TestUndoMarks );
    CPPUNIT_TEST( TestRemoveMarkedCells );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestMarkCell();
    void TestRemoveMarkedCells();
  	void TestUndoMarks();

    void RenderData(vtkPolyData *data);
    
};

#endif
