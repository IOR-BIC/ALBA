/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataToSinglePolyLineTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAPolyDataToSinglePolyLineTEST_H__
#define __CPP_UNIT_vtkALBAPolyDataToSinglePolyLineTEST_H__

#include "albaTest.h"

class vtkALBAPolyDataToSinglePolyLineTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBAPolyDataToSinglePolyLineTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConversion );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestConversion();

};

#endif
