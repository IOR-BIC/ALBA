/*=========================================================================

 Program: MAF2
 Module: vtkMAFCellsFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFCellsFilterTEST_H__
#define __CPP_UNIT_vtkMAFCellsFilterTEST_H__

#include "mafTest.h"

class vtkMAFCellsFilterTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkMAFCellsFilterTest );
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestMarkCell);
    CPPUNIT_TEST(TestUnmarkCell);
    CPPUNIT_TEST(TestToggleCell);
    CPPUNIT_TEST(TestUndoMarks);
    CPPUNIT_TEST(TestGetNumberOfMarkedCells);
    CPPUNIT_TEST(TestGetIdMarkedCell);
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestMarkCell();
    void TestUnmarkCell();
    void TestToggleCell();
    void TestUndoMarks();
    void TestGetNumberOfMarkedCells();
    void TestGetIdMarkedCell();
    
};

#endif
