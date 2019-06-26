/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformFrameTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTransformFrameTest_H__
#define __CPP_UNIT_albaTransformFrameTest_H__

#include "albaTest.h"

class vtkTransform;
class albaMatrix;

/** Test suite for albaTransformFrame */
class albaTransformFrameTest : public albaTest
{
  public: 

    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( albaTransformFrameTest );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestMafTransformFrameConstructorDestructor );
    CPPUNIT_TEST( TestSetInputMafTransformBase );
    CPPUNIT_TEST( TestSetInputMafMatrix );
    CPPUNIT_TEST( TestGetInput );
    CPPUNIT_TEST( TestSetInputFrameMafMatrix );
    CPPUNIT_TEST( TestSetInputFrameMafTransformBase );
    CPPUNIT_TEST( TestGetInputFrame );
    CPPUNIT_TEST( TestSetTargetFrameMafMatrix );
    CPPUNIT_TEST( TestSetTargetFrameMafTransformBase );
    CPPUNIT_TEST( TestGetTargetFrame );
    CPPUNIT_TEST( TestGetMTime );
    CPPUNIT_TEST( TestMatrixTransformationFromInputFrameToTargetFrameWithRendering );
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();
    void TestMatrixTransformationFromInputFrameToTargetFrameWithRendering();

    void TestMafTransformFrameConstructorDestructor();
    
    /** set the materix to be transformed */
    void TestSetInputMafTransformBase();
    void TestSetInputMafMatrix();
    void TestGetInput();

    /**
    Set/Get the input reference system, i.e. the reference system of the 
    input matrix.*/
    void TestSetInputFrameMafMatrix();
    void TestSetInputFrameMafTransformBase();
    void TestGetInputFrame();

    /**
    Set/Get the output reference system, i.e. the reference system of the output
    matrix or the target reference system for point transformation.*/
    void TestSetTargetFrameMafMatrix();
    void TestSetTargetFrameMafTransformBase();
    void TestGetTargetFrame();

    /** 
    Return current modification time, taking inro consideration also
    Input, InputFrame and TargetFrame. */
    void TestGetMTime();


    vtkTransform *m_InputFrameTransform;
    vtkTransform *m_TargetFrameTransform;
    vtkTransform *m_InputMatrixTransform;
    albaMatrix *m_InputMatrix;
    albaMatrix *m_InputFrameMatrix;
    albaMatrix *m_TargetFrameMatrix;

    

};


#endif
