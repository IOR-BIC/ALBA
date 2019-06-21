/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorConstraintTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorConstraintTest_H__
#define __CPP_UNIT_albaInteractorConstraintTest_H__

#include "albaTest.h"

#include "albaInteractorConstraint.h"

class albaInteractorConstraintTest : public albaTest
{
  public:
  CPPUNIT_TEST_SUITE( albaInteractorConstraintTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetConstraintModality );
  CPPUNIT_TEST( TestGetNumberOfDOF );
  CPPUNIT_TEST( TestGetConstraintAxis );
  CPPUNIT_TEST( TestGetConstraintPlane );
  CPPUNIT_TEST( TestGetConstraintPlaneAxes );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestSetGetLowerBound );
  CPPUNIT_TEST( TestSetGetUpperBound );
  CPPUNIT_TEST( TestSetGetBounds );
  CPPUNIT_TEST( TestSetGetMin );
  CPPUNIT_TEST( TestSetGetMax );
  CPPUNIT_TEST( TestSetGetStep );
  CPPUNIT_TEST( TestSetSnapStep );
  CPPUNIT_TEST( TestSetGetSnapArray );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetGetConstraintModality(); 
    void TestGetNumberOfDOF();
    void TestGetConstraintAxis();   
    void TestGetConstraintPlane();
    void TestGetConstraintPlaneAxes();
    void TestReset();    
    void TestSetGetLowerBound();
    void TestSetGetUpperBound();
    void TestSetGetBounds();
    void TestSetGetMin();
    void TestSetGetMax();
    void TestSetGetStep();
    void TestSetSnapStep();
    void TestSetGetSnapArray();
    
};

#endif
