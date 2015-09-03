/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineGraphTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEPolylineGraphTEST_H__
#define __CPP_UNIT_mafVMEPolylineGraphTEST_H__

#include "mafTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for mafVMEPolylineGraph
//------------------------------------------------------------------------------
class mafVMEPolylineGraphTest : public mafTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( mafVMEPolylineGraphTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestSetData );

    //CPPUNIT_TEST( TestCopyFromPolydata );
    //CPPUNIT_TEST( TestCopyToPolydata );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestDynamicAllocation();
    void TestSetData();
//    void TestCopyFromPolydata();
//    void TestCopyToPolydata();

		void CreateExamplePolydata();
    vtkPolyData *m_Polydata ;
};

#endif
