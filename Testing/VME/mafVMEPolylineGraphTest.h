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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for mafVMEPolylineGraph
//------------------------------------------------------------------------------
class mafVMEPolylineGraphTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

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
