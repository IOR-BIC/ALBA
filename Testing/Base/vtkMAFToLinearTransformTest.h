/*=========================================================================

 Program: MAF2
 Module: vtkMAFToLinearTransformTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFToLinearTransformTest_H__
#define __CPP_UNIT_vtkMAFToLinearTransformTest_H__

#include "mafTest.h"

class vtkTransform;
class mafMatrix;

/** Test suite for vtkMAFToLinearTransform */
class vtkMAFToLinearTransformTest : public mafTest
{
  public: 
		 
    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( vtkMAFToLinearTransformTest );
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