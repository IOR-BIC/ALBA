/*=========================================================================

 Program: MAF2
 Module: mafHTMLTemplateParserBlockTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafHTMLTemplateParserBlockTest_H__
#define __CPP_UNIT_mafHTMLTemplateParserBlockTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafHTMLTemplateParserBlockTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafHTMLTemplateParserBlockTest );
  CPPUNIT_TEST( TestConstructorDestuctor );
  CPPUNIT_TEST( TestAddGetVar );
  CPPUNIT_TEST( TestPushGetNthVar );
  CPPUNIT_TEST( TestAddGetBlock );
  CPPUNIT_TEST( TestPushGetNthBlock );
  CPPUNIT_TEST( TestSetGetIfCondition );
  CPPUNIT_TEST( TestSetGetNloop );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestConstructorDestuctor();
    void TestAddGetVar();
    void TestPushGetNthVar();
    void TestAddGetBlock();
    void TestPushGetNthBlock();
    void TestSetGetIfCondition();
    void TestSetGetNloop();
     
};


#endif