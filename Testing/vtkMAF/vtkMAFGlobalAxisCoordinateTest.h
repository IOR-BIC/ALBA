/*=========================================================================

 Program: MAF2
 Module: vtkMAFGlobalAxisCoordinateTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFGlobalAxisCoordinateTEST_H__
#define __CPP_UNIT_vtkMAFGlobalAxisCoordinateTEST_H__

#include "mafTest.h"

/*Test for  vtkMAFGlobalAxisCoordinate*/
class vtkMAFGlobalAxisCoordinateTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkMAFGlobalAxisCoordinateTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    //CPPUNIT_TEST( TestStaticAllocation );
    CPPUNIT_TEST( TestGetComputedUserDefinedValue );
    
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestGetComputedUserDefinedValue();
};

#endif
