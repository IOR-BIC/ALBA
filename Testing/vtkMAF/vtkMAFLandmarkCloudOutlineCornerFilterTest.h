/*=========================================================================

 Program: MAF2
 Module: vtkMAFLandmarkCloudOutlineCornerFilterTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFLandmarkCloudOutlineCornerFilterTEST_H__
#define __CPP_UNIT_vtkMAFLandmarkCloudOutlineCornerFilterTEST_H__

#include "mafTest.h"

class vtkMAFLandmarkCloudOutlineCornerFilterTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE(vtkMAFLandmarkCloudOutlineCornerFilterTest);
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
