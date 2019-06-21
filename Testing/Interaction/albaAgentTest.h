/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAgentTest_H__
#define __CPP_UNIT_albaAgentTest_H__

#include "albaTest.h"

class albaTestAgent;
class albaAgentRouter;

class albaAgentTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaAgentTest );
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

    albaTestAgent *m_Source;
    albaAgentRouter *m_Router;
    albaTestAgent *m_Sink1;
    albaTestAgent *m_Sink2;
    albaTestAgent *m_Sink3;
    albaTestAgent *m_Sink4;

};

#endif
