/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACellsFilterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBACellsFilterTEST_H__
#define __CPP_UNIT_vtkALBACellsFilterTEST_H__

#include "albaTest.h"

class vtkALBACellsFilterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBACellsFilterTest );
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
