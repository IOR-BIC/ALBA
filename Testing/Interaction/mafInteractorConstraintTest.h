/*=========================================================================

 Program: MAF2
 Module: mafInteractorConstraintTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorConstraintTest_H__
#define __CPP_UNIT_mafInteractorConstraintTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafInteractorConstraint.h"

class mafInteractorConstraintTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafInteractorConstraintTest );
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
