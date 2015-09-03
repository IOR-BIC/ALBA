/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMELANDMARKTEST_H__
#define __CPP_UNIT_MAFVMELANDMARKTEST_H__

#include "mafTest.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"

/** Test for mafVMELandmark.*/
class mafVMELandmarkTest : public mafTest
{

  public:
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( mafVMELandmarkTest );
    CPPUNIT_TEST( DynamicAllocationTest );
    CPPUNIT_TEST( SetTimeStampTest );
    CPPUNIT_TEST( SetPointTest );
    CPPUNIT_TEST( SetPointTest2 );
    CPPUNIT_TEST( SetRadiusTest );
    CPPUNIT_TEST( SetSphereResolutionTest );
    CPPUNIT_TEST( SetLandmarkVisibilityTest );
    CPPUNIT_TEST( CanReparentToTest );
    CPPUNIT_TEST( SetMatrixTest );
    CPPUNIT_TEST( DeepCopyTest ) ;
    CPPUNIT_TEST_SUITE_END();

  protected:
    void DynamicAllocationTest();
    void SetTimeStampTest();
    void SetPointTest();
    void SetPointTest2();
    void SetRadiusTest();
    void SetSphereResolutionTest();
    void SetLandmarkVisibilityTest();
    void CanReparentToTest();
    void SetMatrixTest();
    void DeepCopyTest();

};

#endif
