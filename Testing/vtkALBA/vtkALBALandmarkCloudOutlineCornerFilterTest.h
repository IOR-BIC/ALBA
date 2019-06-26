/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALandmarkCloudOutlineCornerFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBALandmarkCloudOutlineCornerFilterTEST_H__
#define __CPP_UNIT_vtkALBALandmarkCloudOutlineCornerFilterTEST_H__

#include "albaTest.h"

class vtkALBALandmarkCloudOutlineCornerFilterTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE(vtkALBALandmarkCloudOutlineCornerFilterTest);
    CPPUNIT_TEST(TestInput);
    CPPUNIT_TEST(TestGetSet);
    CPPUNIT_TEST(TestBounds);
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestInput();
    void TestGetSet();
    void TestBounds();    
};

#endif
