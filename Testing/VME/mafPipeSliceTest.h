/*=========================================================================

 Program: MAF2
 Module: mafPipeSliceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeSliceTest_H__
#define __CPP_UNIT_mafPipeSliceTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafPipeSliceTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafPipeSliceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestGetOrigin1 ); 
  CPPUNIT_TEST( TestGetOrigin2 );
  CPPUNIT_TEST( TestGetNormal1 );
  CPPUNIT_TEST( TesGetNormal2 );
  CPPUNIT_TEST( TestGetSlice );   
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestGetOrigin1(); 
  void TestGetOrigin2();
  void TestGetNormal1();
  void TesGetNormal2();
  void TestGetSlice();  

};

#endif
