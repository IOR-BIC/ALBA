/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAToLinearTransformTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAToLinearTransformTest_H__
#define __CPP_UNIT_vtkALBAToLinearTransformTest_H__

#include "albaTest.h"

class vtkTransform;
class albaMatrix;

/** Test suite for vtkALBAToLinearTransform */
class vtkALBAToLinearTransformTest : public albaTest
{
  public: 
		 
    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( vtkALBAToLinearTransformTest );
    CPPUNIT_TEST( TestSmartAllocation );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestSetGetInputMatrix );
    CPPUNIT_TEST( TestSetGetInputTransform );
    CPPUNIT_TEST( TestInverse );
    CPPUNIT_TEST( TestMTime );
    CPPUNIT_TEST( TestMakeTransform );
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestSmartAllocation();
    void TestDynamicAllocation();
    void TestSetGetInputMatrix();
    void TestSetGetInputTransform();
    void TestInverse();
    void TestMTime();
    void TestMakeTransform();

};

#endif