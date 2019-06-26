/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentThreadedTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAgentThreadedTest_H__
#define __CPP_UNIT_albaAgentThreadedTest_H__

#include "albaTest.h"

#include "albaObserver.h"

class albaEventBase;

class albaAgentThreadedTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaAgentThreadedTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestSetGetThreaded );
  CPPUNIT_TEST( TestUpdate );
  CPPUNIT_TEST( TestAsyncSendEvent );
  CPPUNIT_TEST( TestAsyncInvokeEvent );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();   
  void TestSetGetThreaded();
  void TestUpdate();
  void TestOnEvent();
  void TestAsyncSendEvent();
  void TestAsyncInvokeEvent();

};

#endif
