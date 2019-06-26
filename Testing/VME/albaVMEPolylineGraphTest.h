/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineGraphTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEPolylineGraphTEST_H__
#define __CPP_UNIT_albaVMEPolylineGraphTEST_H__

#include "albaTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for albaVMEPolylineGraph
//------------------------------------------------------------------------------
class albaVMEPolylineGraphTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( albaVMEPolylineGraphTest );
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
