/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMELANDMARKTEST_H__
#define __CPP_UNIT_ALBAVMELANDMARKTEST_H__

#include "albaTest.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"

/** Test for albaVMELandmark.*/
class albaVMELandmarkTest : public albaTest
{

  public:

    CPPUNIT_TEST_SUITE( albaVMELandmarkTest );
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
