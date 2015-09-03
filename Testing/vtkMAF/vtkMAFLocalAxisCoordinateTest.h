/*=========================================================================

 Program: MAF2
 Module: vtkMAFLocalAxisCoordinateTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFLocalAxisCoordinateTEST_H__
#define __CPP_UNIT_vtkMAFLocalAxisCoordinateTEST_H__

#include "mafTest.h"

/*Test for  vtkMAFLocalAxisCoordinate*/
class vtkMAFLocalAxisCoordinateTest : public mafTest
{
  public:
  
    CPPUNIT_TEST_SUITE( vtkMAFLocalAxisCoordinateTest );
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
