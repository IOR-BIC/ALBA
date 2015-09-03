/*=========================================================================

 Program: MAF2
 Module: mafAgentTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAgentTest_H__
#define __CPP_UNIT_mafAgentTest_H__

#include "mafTest.h"

class mafTestAgent;
class mafAgentRouter;

class mafAgentTest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafAgentTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST(  TestAddRemoveObserver );
  CPPUNIT_TEST(  TestRemoveAllObservers );
  CPPUNIT_TEST(  TestHasObservers );
  CPPUNIT_TEST(  TestGetObservers );
  CPPUNIT_TEST(  TestInitialize );
  CPPUNIT_TEST(  TestShutdown );
  CPPUNIT_TEST(  TestIsInitialized );
  CPPUNIT_TEST(  TestSetGetName );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestAddRemoveObserver();
    void TestRemoveObserver();
    void TestRemoveAllObservers();
    void TestHasObservers();
    void TestGetObservers();
    void TestInitialize();
    void TestShutdown();
    void TestIsInitialized();
    void TestSetGetName();

  private:

    mafTestAgent *m_Source;
    mafAgentRouter *m_Router;
    mafTestAgent *m_Sink1;
    mafTestAgent *m_Sink2;
    mafTestAgent *m_Sink3;
    mafTestAgent *m_Sink4;

};

#endif
