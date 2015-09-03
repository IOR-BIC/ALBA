/*=========================================================================

 Program: MAF2
 Module: vtkMAFDOFMatrixTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFDOFMatrixTest_H__
#define __CPP_UNIT_vtkMAFDOFMatrixTest_H__

#include "mafTest.h"

class vtkMAFDOFMatrix;

class vtkMAFDOFMatrixTest : public mafTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkMAFDOFMatrixTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST(TestDeepCopy);
    CPPUNIT_TEST(TestReset);
    CPPUNIT_TEST(TestSetGetState);
    CPPUNIT_TEST(TestSetGetLowerBound);
    CPPUNIT_TEST(TestSetGetUpperBound);
    CPPUNIT_TEST(TestSetGetMin);
    CPPUNIT_TEST(TestSetGetMax);
    CPPUNIT_TEST(TestSetGetStep);
    CPPUNIT_TEST(TestSetGetArray);
    CPPUNIT_TEST(TestGetDOFNumber);
    CPPUNIT_TEST(TestGetConstrainAxis);
    CPPUNIT_TEST(TestGetConstrainPlane);
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestConstructorDestructor();
    void TestDeepCopy();
    void TestReset();
    void TestSetGetState();
    void TestSetGetLowerBound();
    void TestSetGetUpperBound();
    void TestSetGetMin();
    void TestSetGetMax();
    void TestSetGetStep();
    void TestSetGetArray();
    void TestGetDOFNumber();
    void TestGetConstrainAxis();
    void TestGetConstrainPlane();
};

#endif
