/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALocalAxisCoordinateTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBALocalAxisCoordinateTEST_H__
#define __CPP_UNIT_vtkALBALocalAxisCoordinateTEST_H__

#include "albaTest.h"

/*Test for  vtkALBALocalAxisCoordinate*/
class vtkALBALocalAxisCoordinateTest : public albaTest
{
  public:
  
    CPPUNIT_TEST_SUITE( vtkALBALocalAxisCoordinateTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    //CPPUNIT_TEST( TestStaticAllocation );
    CPPUNIT_TEST( TestMatrixAccessors );
    CPPUNIT_TEST( TestDatasetAccessors );
    CPPUNIT_TEST( TestGetComputedUserDefinedValue );
    
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestMatrixAccessors();
    void TestDatasetAccessors();
    void TestGetComputedUserDefinedValue();
};

#endif
