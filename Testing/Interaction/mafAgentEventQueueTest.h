/*=========================================================================

 Program: MAF2
 Module: mafAgentEventQueueTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAgentEventQueueTest_H__
#define __CPP_UNIT_mafAgentEventQueueTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafAgentEventQueueTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafAgentEventQueueTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestPushEvent );
  CPPUNIT_TEST( TestPeekEvent );
  CPPUNIT_TEST( TestPeekLastEvent );
  CPPUNIT_TEST( TestGetQueueSize );
  CPPUNIT_TEST( TestIsQueueEmpty );
  CPPUNIT_TEST( TestDispatchEvents );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();   
  void TestPushEvent();
  void TestPeekEvent();
  void TestPeekLastEvent();
  void TestGetQueueSize();
  void TestIsQueueEmpty();
  void TestDispatchEvents();

};

#endif
