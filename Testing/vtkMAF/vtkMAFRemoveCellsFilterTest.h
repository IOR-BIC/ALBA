/*=========================================================================

 Program: MAF2
 Module: vtkMAFRemoveCellsFilterTest
 Authors: Matteo Giacomoni, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFRemoveCellsFilterTEST_H__
#define __CPP_UNIT_vtkMAFRemoveCellsFilterTEST_H__

#include "mafTest.h"

class vtkPolyData;

class vtkMAFRemoveCellsFilterTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkMAFRemoveCellsFilterTest );
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
