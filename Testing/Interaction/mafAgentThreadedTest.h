/*=========================================================================

 Program: MAF2
 Module: mafAgentThreadedTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAgentThreadedTest_H__
#define __CPP_UNIT_mafAgentThreadedTest_H__

#include "mafTest.h"

#include "mafObserver.h"

class mafEventBase;

class mafAgentThreadedTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafAgentThreadedTest );
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
